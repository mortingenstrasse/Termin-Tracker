import AppKit
import CoreMIDI
import CoreAudio

struct MIDIInputEvent { let status:Int; let note:Int; let value:Int; let time:Double }

// CoreMIDI converts available MIDI sources to MIDI 1.0 UMP. One connection per source.
final class MIDIInput {
    private var client:MIDIClientRef=0, port:MIDIPortRef=0
    private var connected=Set<MIDIEndpointRef>()
    weak var tracker:Tracker?
    init(_ tracker:Tracker){
        self.tracker=tracker
        let status=MIDIClientCreateWithBlock("Termin Tracker" as CFString,&client){[weak self] _ in DispatchQueue.main.async{self?.refresh()}}
        guard status==noErr else{tracker.midiStatus="CoreMIDI unavailable (\(status))";return}
        let result=MIDIInputPortCreateWithProtocol(client,"Termin Tracker Input" as CFString,._1_0,&port){[weak self] list,_ in
            let events=Self.decode(list)
            if !events.isEmpty{DispatchQueue.main.async{[weak self] in for event in events{self?.tracker?.receiveMIDI(event)}}}
        }
        guard result==noErr else{tracker.midiStatus="MIDI input unavailable (\(result))";return}
        refresh()
    }
    deinit{if port != 0{MIDIPortDispose(port)};if client != 0{MIDIClientDispose(client)}}
    static func decode(_ list:UnsafePointer<MIDIEventList>)->[MIDIInputEvent]{
        var events=[MIDIInputEvent]();let offset=MemoryLayout<MIDIEventList>.offset(of:\.packet)!
        var packet=UnsafeRawPointer(list).advanced(by:offset).assumingMemoryBound(to:MIDIEventPacket.self)
        let sizes=[1,1,1,2,2,4,1,1,2,2,2,3,3,4,4,4]
        for _ in 0..<list.pointee.numPackets{
            let timestamp=packet.pointee.timeStamp
            let time=Double(AudioConvertHostTimeToNanos(timestamp==0 ? AudioGetCurrentHostTime():timestamp))/1e9
            let count=Int(packet.pointee.wordCount)
            let words=UnsafeRawPointer(packet).advanced(by:MemoryLayout<MIDIEventPacket>.offset(of:\.words)!).assumingMemoryBound(to:UInt32.self)
            var i=0;while i<count{let word=words[i],type=Int(word>>28);if type==2{let status=Int((word>>16)&255),note=Int((word>>8)&127),value=Int(word&127);if [0x80,0x90,0xB0].contains(status&0xf0){events.append(MIDIInputEvent(status:status,note:note,value:value,time:time))}};i+=sizes[type]}
            packet=UnsafePointer(MIDIEventPacketNext(packet))
        };return events
    }
    func refresh(){guard port != 0 else{return};let available=Set((0..<MIDIGetNumberOfSources()).map{MIDIGetSource($0)}.filter{$0 != 0});let removed=connected.subtracting(available);if !removed.isEmpty{tracker?.midiPanic()};for source in removed{MIDIPortDisconnectSource(port,source);connected.remove(source)};for source in available.subtracting(connected){if MIDIPortConnectSource(port,source,nil)==noErr{connected.insert(source)}};let names=connected.sorted().map{source -> String in var s:Unmanaged<CFString>?;if MIDIObjectGetStringProperty(source,kMIDIPropertyDisplayName,&s)==noErr,let name=s?.takeRetainedValue(){return name as String};return "MIDI source"};tracker?.midiStatus=names.isEmpty ? "No MIDI input connected":names.joined(separator:", ");tracker?.midiSourceCount=names.count;tracker?.needsDisplay=true}
}

struct MIDIChordCapture {
    let pattern:Int,track:Int,row:Int,instrument:Int,micro:Int,live:Bool,started:Double,window:Double
    var notes:[Int:Int]
}

func midiChordLabel(_ notes:[Int])->String {
    let sorted=notes.sorted();guard let bass=sorted.first,sorted.count>1 else{return ""}
    let pitches=Set(sorted.map{$0%12})
    for root in [bass%12]+pitches.sorted().filter({$0 != bass%12}){for chord in chords where Set(chord.1.map{($0+root)%12})==pitches{let suffix=chord.0=="Major" ? "":chord.0=="Minor" ? "m":chord.0;return "\(names[root])\(suffix)-\(bass/12-1)"}}
    return "\(names[bass%12]) chord-\(bass/12-1)"
}

extension Tracker {
    func midiPanic(){computerHeld.removeAll();midiHeld.removeAll();midiSustained.removeAll();midiPedal=false;tm_midi_panic();flushMIDIChord();finalizeDuration()}
    func midiConfigSoft(_ i:Int){switch i{case 0:midiChordWindow=max(25,midiChordWindow-5);case 1:midiChordWindow=min(150,midiChordWindow+5);case 2:midiPanic();case 3:midiMicroQuantize.toggle();default:break};needsDisplay=true}
    func midiTarget()->Int?{if home||(modal != 0&&modal != 7){return nil};if page==2||page==7||page==12||samplePage{return slots[instrumentRow].name=="EMPTY" ? nil:instrumentRow};guard page==0,track<activeTrackCount else{return nil};let i=cell.instrument ?? trackDefaults[track];return i>=0 && slots[i].name != "EMPTY" ? i:nil}
    func finishMIDIRelease(_ time:Double){
        let down=midiHeld.union(midiSustained).union(computerHeld.values)
        if let capture=midiCapture,Set(capture.notes.keys).isDisjoint(with:down){flushMIDIChord()}
        if let captured=midiReleaseCapture,Set(captured.notes.keys).isDisjoint(with:down){finalizeDuration(at:time)}
    }
    func updateLiveDuration(){guard playing&&liveRec, let capture=midiReleaseCapture else{return};writeDuration(capture,at:Double(AudioConvertHostTimeToNanos(AudioGetCurrentHostTime()))/1e9)}
    func finalizeDuration(at time:Double?=nil){guard let capture=midiReleaseCapture else{return};midiReleaseCapture=nil;writeDuration(capture,at:time ?? Double(AudioConvertHostTimeToNanos(AudioGetCurrentHostTime()))/1e9)}
    private func writeDuration(_ capture:MIDIChordCapture,at time:Double){guard capture.live,var p=project.patterns[capture.pattern],capture.track<p.activeTrackCount,capture.row<p.length else{return};var c=p.cells[capture.track][capture.row];guard c.notes==capture.notes.keys.sorted() else{return};let maxLength=max(1,min(511,(p.length-capture.row)*4-capture.micro/25));let quarters=max(1,min(maxLength,Int(min(511,max(0,time-capture.started)*project.bpm/15*4).rounded())));if c.duration != quarters{c.duration=quarters;p.cells[capture.track][capture.row]=c;project.patterns[capture.pattern]=p;push(capture.pattern,capture.track,capture.row,c);dirty=true;needsDisplay=true}}
    func receiveMIDI(_ e:MIDIInputEvent,computer:Bool=false){
        let type=e.status&0xf0,n=e.note
        if type==0xB0{if n==64{midiPedal=e.value>=64;if !midiPedal{for note in midiSustained where !midiHeld.contains(note) && !computerHeld.values.contains(note){tm_midi_note(Int32(note),0,0)};midiSustained.removeAll();finishMIDIRelease(e.time)}}else if n==120||n==123{midiPanic()};return}
        if type==0x80 || e.value==0{if !computer{midiHeld.remove(n);if midiPedal{midiSustained.insert(n)}};if !midiHeld.contains(n)&&(!midiSustained.contains(n))&&(!computerHeld.values.contains(n)){tm_midi_note(Int32(n),0,0)};finishMIDIRelease(e.time);return}
        followSongPattern();guard let ins=midiTarget() else{return}
        if !computer{midiHeld.insert(n);midiSustained.remove(n)};tm_midi_note(Int32(n),Int32(ins),Int32(e.value));midiLastNote=noteName(n);needsDisplay=true
        guard page==0,rec,(!playing||liveRec),(!liveRec||armed.isEmpty||armed.contains(track)) else{return}
        if var pending=midiCapture, pending.pattern==project.selected,pending.track==track,pending.instrument==ins,e.time-pending.started<=pending.window,e.time>=pending.started,pending.notes[n]==nil,pending.notes.count<8{pending.notes[n]=e.value;midiCapture=pending;return}
        flushMIDIChord();finalizeDuration(at:e.time)
        let live=playing&&liveRec;var targetRow=row,offset=0
        if live{let pos=tm_record_position();guard pos != UInt32.max,Int(pos>>17)==project.selected else{return};targetRow=min(pattern.length-1,Int((pos>>10)&127));let phase=Double(pos&1023)/1000;let quantized=midiMicroQuantize ? Int((phase*4).rounded())*25:Int(phase.rounded())*100;if quantized==100{targetRow=(targetRow+1)%pattern.length}else{offset=quantized}}
        let window=min(Double(midiChordWindow)/1000,live ? 60/project.bpm/4*0.75:1)
        midiCapture=MIDIChordCapture(pattern:project.selected,track:track,row:targetRow,instrument:ins,micro:offset,live:live,started:e.time,window:window,notes:[n:e.value]);midiCaptureGeneration+=1;let generation=midiCaptureGeneration
        DispatchQueue.main.asyncAfter(deadline:.now()+window){[weak self] in guard let self,self.midiCaptureGeneration==generation else{return};self.flushMIDIChord()}
    }
    func flushMIDIChord(){guard let pending=midiCapture else{return};midiCapture=nil;midiCaptureGeneration+=1;guard var p=project.patterns[pending.pattern],pending.track<p.activeTrackCount,pending.row<p.length else{return};let notes=pending.notes.keys.sorted();guard !notes.isEmpty else{return};checkpoint();var c=p.cells[pending.track][pending.row];c.notes=notes;c.duration=pending.live ? 1:nil;c.noteOff=nil;c.label=midiChordLabel(notes);c.harmonicRoot=notes.count>1 ? notes[0]:nil;c.instrument=pending.instrument;c.velocity=max(1,Int((Double(pending.notes.values.reduce(0,+))/Double(notes.count)*100/127).rounded()));if pending.live{c.micro=pending.micro};p.cells[pending.track][pending.row]=c;project.patterns[pending.pattern]=p;if pending.live{midiReleaseCapture=pending};push(pending.pattern,pending.track,pending.row,c);if project.selected==pending.pattern&&track==pending.track{row=pending.live ? pending.row : min(p.length-1,pending.row+project.step);anchor=nil};message="MIDI \(notes.count>1 ? c.label:noteName(notes[0]))  ·  \(notes.count) note(s)";needsDisplay=true}
}

func midiSelfTest(){
    precondition(midiChordLabel([60,64,67])=="C-4");precondition(midiChordLabel([65,68,72,75])=="Fm7-4")
    let t=Tracker(frame:NSRect(x:0,y:0,width:1440,height:900));t.rec=true;t.assignInstrument(3,wholeTrack:false);t.project.step=2
    t.receiveMIDI(MIDIInputEvent(status:0x90,note:60,value:100,time:1));t.receiveMIDI(MIDIInputEvent(status:0x90,note:64,value:80,time:1.02));t.receiveMIDI(MIDIInputEvent(status:0x90,note:67,value:120,time:1.045));t.flushMIDIChord();precondition(t.pattern.cells[0][0].notes==[60,64,67]&&t.row==2&&t.pattern.cells[0][0].velocity==79)
    t.receiveMIDI(MIDIInputEvent(status:0x90,note:62,value:100,time:2));t.receiveMIDI(MIDIInputEvent(status:0x90,note:65,value:100,time:2.2));t.flushMIDIChord();precondition(t.pattern.cells[0][2].notes==[62]&&t.pattern.cells[0][4].notes==[65]);t.midiPanic()
    t.project.bpm=120;t.midiCapture=MIDIChordCapture(pattern:0,track:0,row:8,instrument:3,micro:0,live:true,started:10,window:0.065,notes:[60:100]);t.flushMIDIChord();t.finishMIDIRelease(10.5);precondition(t.pattern.cells[0][8].duration==16&&t.pattern.heldCell(track:0,row:11)?.notes==[60]&&t.pattern.heldCell(track:0,row:12)==nil)
}
