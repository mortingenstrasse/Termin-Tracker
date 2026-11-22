import AppKit
import CoreAudio

// Physical ANSI key positions: two rows of a compact piano (C to C).
let computerPiano:[(UInt16,String,Int)]=[(0,"A",0),(13,"W",1),(1,"S",2),(14,"E",3),(2,"D",4),(3,"F",5),(17,"T",6),(5,"G",7),(16,"Y",8),(4,"H",9),(32,"U",10),(38,"J",11),(40,"K",12)]

extension Tracker {
    func keyboardTime()->Double{Double(AudioConvertHostTimeToNanos(AudioGetCurrentHostTime()))/1e9}
    func toggleComputerKeyboard(){
        if computerKeyboard{releaseComputerKeys();computerKeyboard=false;message="Keyboard mode off"}
        else{guard !home,!audioSetupVisible,modal==0 else{return};flushMIDIChord();computerKeyboard=true;footer=nil;field=0;anchor=nil;message="Keyboard mode on • R Record • R+Space Live"}
        needsDisplay=true
    }
    // Consume mapped keys before global tracker commands and before flushing a MIDI chord.
    func computerKeyDown(_ event:NSEvent,at time:Double?=nil)->Bool{
        let code=event.keyCode,flags=event.modifierFlags
        if code==40 && flags.contains(.command) && !flags.contains(.option) && !flags.contains(.control){if !event.isARepeat{toggleComputerKeyboard()};return true}
        guard computerKeyboard,!home,!audioSetupVisible,modal==0,!flags.contains(.command) else{return false}
        if code==6||code==7{if !event.isARepeat{octave=max(0,min(8,octave+(code==6 ? -1:1)));needsDisplay=true};return true}
        guard let key=computerPiano.first(where:{$0.0==code}) else{return false}
        guard !event.isARepeat,computerHeld[code]==nil,!flags.contains(.option),!flags.contains(.control) else{return true}
        followSongPattern()
        guard midiTarget() != nil else{message="Assign an instrument to the selected track first";needsDisplay=true;return true}
        let note=min(127,(octave+1)*12+key.2)
        computerHeld[code]=note
        receiveMIDI(MIDIInputEvent(status:0x90,note:note,value:100,time:time ?? keyboardTime()),computer:true)
        needsDisplay=true;return true
    }
    @discardableResult func computerKeyUp(_ code:UInt16,at time:Double?=nil)->Bool{
        guard let note=computerHeld.removeValue(forKey:code) else{return computerKeyboard&&(computerPiano.contains{$0.0==code}||code==6||code==7)}
        if !computerHeld.values.contains(note){receiveMIDI(MIDIInputEvent(status:0x80,note:note,value:0,time:time ?? keyboardTime()),computer:true)}
        needsDisplay=true;return true
    }
    func releaseComputerKeys(){
        let time=keyboardTime()
        for code in Array(computerHeld.keys){_ = computerKeyUp(code,at:time)}
        needsDisplay=true
    }
    func drawComputerKeyboard(){
        let w=min(350,bounds.width-24),x=bounds.width-w-12,y=footerY+63
        rect(0,y-2,bounds.width,76,trackerBackground)
        rect(x,y,w,69,gray(0.18));border(x,y,w,69,teal)
        let playingNotes=Set(computerHeld.values)
        let label=playingNotes.sorted().map(noteName).joined(separator:" ")
        txt("KEYBOARD MODE  •  OCT \(octave)",x+9,y+5,11,teal,true)
        let status=label.isEmpty ? "⌘K OFF":label
        let cap=max(5,Int((w-196)/7));txt(String(status.prefix(cap)),x+190,y+5,11,yellow,true)
        let white=[0,2,4,5,7,9,11,12],black=[1,3,6,8,10],keyWidth=(w-18)/8,top=y+22
        for (i,semi) in white.enumerated(){let key=computerPiano.first{$0.2==semi}!,xx=x+9+CGFloat(i)*keyWidth,active=computerHeld[key.0] != nil;rect(xx,top,keyWidth-1,39,active ? teal:gray(0.87));centered(key.1,xx,top+25,keyWidth-1,10,active ? .black:gray(0.28),true)}
        for semi in black{let key=computerPiano.first{$0.2==semi}!,i=white.lastIndex(where:{$0<semi})!,xx=x+9+(CGFloat(i)+0.72)*keyWidth,active=computerHeld[key.0] != nil,bw=keyWidth*0.54;rect(xx,top,bw,24,active ? yellow:gray(0.12));centered(key.1,xx,top+5,bw,9,active ? .black:terminText,true)}
        // Help stays separate from the grid and the existing eight footer buttons.
        if x>490{txt("Z / X OCTAVE  •  R REC / R+SPACE LIVE  •  ⌘K EXIT",12,y+28,11,gray(0.72))}
    }
    // Follow the engine's atomic pattern/row snapshot, not the selected Song list row.
    // Do not change the edit target underneath an open modal or Fill preview.
    func followSongPattern(){
        guard playing,songFollowing,page==0,!home,!audioSetupVisible,modal==0,fillPreview==nil else{return}
        let pos=tm_record_position();guard pos != UInt32.max else{return}
        let id=Int(pos>>17),step=Int((pos>>10)&127)
        guard let p=project.patterns[id] else{return}
        if id != project.selected {
            flushMIDIChord()
            project.selected=id;row=min(step,p.length-1);scroll=0;anchor=nil;footer=nil
            clampTrack();tm_select_pattern(Int32(id),Int32(p.length))
        }
        followVisibleRow(length:p.length,playhead:step);needsDisplay=true
    }
}

func computerKeyboardSelfTest(){
    tm_init(48000)
    let t=Tracker(frame:NSRect(x:0,y:0,width:1440,height:900));t.rec=true;t.assignInstrument(9,wholeTrack:false)
    func event(_ code:UInt16,_ ch:String,_ flags:NSEvent.ModifierFlags=[],_ down:Bool=true,_ repeatKey:Bool=false)->NSEvent{NSEvent.keyEvent(with:down ? .keyDown:.keyUp,location:.zero,modifierFlags:flags,timestamp:0,windowNumber:0,context:nil,characters:ch,charactersIgnoringModifiers:ch,isARepeat:repeatKey,keyCode:code)!}
    t.keyDown(with:event(40,"k",.command));precondition(t.computerKeyboard&&t.modal==0)
    // Three near-simultaneous physical keydowns must become ONE chord, not three lines.
    _ = t.computerKeyDown(event(0,"a"),at:1);_ = t.computerKeyDown(event(2,"d"),at:1.01);_ = t.computerKeyDown(event(5,"g"),at:1.02)
    _ = t.computerKeyDown(event(0,"a",[],true,true),at:1.03)
    precondition(t.computerHeld.count==3&&t.midiCapture?.notes.count==3)
    t.keyUp(with:event(0,"a",[],false));t.keyUp(with:event(2,"d",[],false));t.keyUp(with:event(5,"g",[],false))
    precondition(t.pattern.cells[0][0].notes==[60,64,67]&&t.row==1&&t.computerHeld.isEmpty)
    _ = t.computerKeyDown(event(6,"z"),at:2);_ = t.computerKeyDown(event(4,"h"),at:2.1)
    precondition(t.octave==3 && !t.home && t.computerHeld[4]==57)
    _ = t.computerKeyDown(event(7,"x"),at:2.2);precondition(t.octave==4&&t.page==0)
    _ = t.computerKeyUp(4,at:2.3);precondition(t.pattern.cells[0][1].notes==[57])
    // Real rendered transport with two different pattern lengths.
    var p0=t.pattern;p0.length=4;var p1=Pattern();p1.length=8;p1.trackInstruments=p0.trackInstruments
    t.project.patterns=[0:p0,1:p1];t.project.song=[0,1];t.project.selected=0;t.project.bpm=120;t.sync()
    t.selectPage(1);t.keyDown(with:event(15,"r"));t.keyDown(with:event(49," "));t.keyUp(with:event(49," ",[],false));t.keyUp(with:event(15,"r",[],false));t.selectPage(0)
    precondition(t.playing&&t.liveRec&&t.rec&&t.songFollowing)
    var l=[Float](repeating:0,count:256),r=l
    func render(_ blocks:Int){for _ in 0..<blocks{l.withUnsafeMutableBufferPointer{lp in r.withUnsafeMutableBufferPointer{rp in tm_render(lp.baseAddress,rp.baseAddress,256)}}}}
    render(1);t.followSongPattern();precondition(t.project.selected==0)
    render(95);t.followSongPattern();precondition(t.project.selected==1&&t.pattern.length==8)
    // Keyboard live recording path uses the actual playhead, and release time sets duration.
    _ = t.computerKeyDown(event(0,"a"),at:10);t.flushMIDIChord()
    let onset=t.midiReleaseCapture!.row
    _ = t.computerKeyUp(0,at:10.5)
    precondition(t.project.patterns[1]!.cells[0][onset].duration==16)
    precondition(t.project.patterns[1]!.heldCell(track:0,row:onset+1)?.notes==[60])
    t.togglePlay();render(1)
    _ = t.computerKeyDown(event(1,"s"),at:11);t.keyDown(with:event(40,"k",.command))
    precondition(!t.computerKeyboard&&t.computerHeld.isEmpty&&t.midiCapture==nil)
    // Manual duration still works with unmodified K.
    t.row=onset;t.keyDown(with:event(40,"k"));precondition(t.modal==9);t.keyDown(with:event(53,""))
    print("Computer keyboard: chord capture, physical mapping, octave, live duration, note release and song follow passed")
}
