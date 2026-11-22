import AppKit
import AVFoundation
import CoreAudio

struct AudioDeviceSummary {
    let name:String,rate:Double
    static func current(input:Bool)->AudioDeviceSummary {
        var address=AudioObjectPropertyAddress(mSelector:input ? kAudioHardwarePropertyDefaultInputDevice:kAudioHardwarePropertyDefaultOutputDevice,mScope:kAudioObjectPropertyScopeGlobal,mElement:kAudioObjectPropertyElementMain)
        var id=AudioDeviceID(0),size=UInt32(MemoryLayout<AudioDeviceID>.size)
        guard AudioObjectGetPropertyData(AudioObjectID(kAudioObjectSystemObject),&address,0,nil,&size,&id)==noErr,id != 0 else{return .init(name:input ? "No input device":"No output device",rate:0)}
        address.mSelector=kAudioObjectPropertyName
        var name:Unmanaged<CFString>?;size=UInt32(MemoryLayout<Unmanaged<CFString>?>.size)
        let ok=AudioObjectGetPropertyData(id,&address,0,nil,&size,&name)==noErr
        let label=ok ? (name?.takeRetainedValue() as String? ?? "Default device"):"Default device"
        address.mSelector=kAudioDevicePropertyNominalSampleRate
        var rate=0.0;size=UInt32(MemoryLayout<Double>.size)
        if AudioObjectGetPropertyData(id,&address,0,nil,&size,&rate) != noErr{rate=0}
        return .init(name:label,rate:rate)
    }
    var description:String{rate>0 ? "\(name)  /  \(Int(rate)) Hz":name}
}

extension Tracker {
    var microphoneStatus:String {
        switch AVCaptureDevice.authorizationStatus(for:.audio){case .authorized:return "AUTHORIZED / READY";case .notDetermined:return "NOT YET REQUESTED";case .denied:return "DENIED / OUTPUT ONLY";case .restricted:return "RESTRICTED / OUTPUT ONLY";@unknown default:return "UNAVAILABLE"}
    }
    func refreshAudioSetup(){audioInputSummary=AudioDeviceSummary.current(input:true).description;audioOutputSummary=AudioDeviceSummary.current(input:false).description;needsDisplay=true}
    func openAudioSetup(){audioSetupVisible=true;audioSetupChoice=0;audioSetupMessage="";refreshAudioSetup()}
    func audioSetupActivate(){guard !audioSetupBusy else{return};startAudioSession?(audioSetupChoice==0)}
    func audioSetupKey(_ e:NSEvent){let c=e.charactersIgnoringModifiers?.lowercased() ?? "";if e.modifierFlags.contains(.command)&&c=="q"{if confirmReplace(){NSApp.terminate(nil)};return};guard !audioSetupBusy else{return};switch e.keyCode{case 126,123:audioSetupChoice=0;case 125,124:audioSetupChoice=1;case 36,76:audioSetupActivate();case 53:audioSetupVisible=false;queuedHomeAction=nil;default:if c=="r"{refreshAudioSetup()}};needsDisplay=true}
    func audioSetupButtons()->[NSRect]{let w=min(680,bounds.width-80),x=(bounds.width-w)/2,y=min(bounds.height-145,bounds.height*0.57);return [NSRect(x:x,y:y,width:w,height:44),NSRect(x:x,y:y+54,width:w,height:44)]}
    func audioSetupMouse(_ p:NSPoint){guard !audioSetupBusy else{return};if let i=audioSetupButtons().firstIndex(where:{$0.contains(p)}){audioSetupChoice=i;audioSetupActivate()}}
    func drawAudioSetup(){
        let w=bounds.width,h=bounds.height,buttons=audioSetupButtons(),x=buttons[0].minX,bw=buttons[0].width,top=max(54,h*0.12)
        rect(0,0,w,h,trackerBackground);rect(8,8,28,28,gray(0.29));centered("×",8,8,28,12,terminText,true)
        centered("TERMIN TRACKER  /  AUDIO SETUP",0,top,w,12,teal,true)
        let data=[("OUTPUT",audioOutputSummary),("RECORD INPUT",audioInputSummary),("USB / MIDI",midiStatus),("INPUT PERMISSION",microphoneStatus),("AUDIO ENGINE",audioEngineReady ? "RUNNING":"NOT STARTED")]
        for (i,item) in data.enumerated(){let y=top+42+CGFloat(i)*32;rect(x,y,bw,28,gray(0.19));txt(item.0,x+10,y+7,12,gray(0.78),true);let cap=max(8,Int((bw-195)/7));txt(String(item.1.prefix(cap)),x+175,y+7,12,i==3 ? yellow:teal)}
        for (i,b) in buttons.enumerated(){rect(b.minX,b.minY,b.width,b.height,i==audioSetupChoice ? gray(0.32):gray(0.19));border(b.minX,b.minY,b.width,b.height,i==audioSetupChoice ? teal:gray(0.36));centered(i==0 ? "START AUDIO + ENABLE LIVE RECORD":"START AUDIO / PLAYBACK ONLY",b.minX,b.minY+14,b.width,12,i==audioSetupChoice ? teal:terminText,true)}
        centered(audioSetupBusy ? "Waiting for macOS microphone permission…" : audioSetupMessage.isEmpty ? "↑ ↓ Select    Enter Start    R Refresh devices":"\(audioSetupMessage)",0,buttons[1].maxY+16,w,12,yellow)
        centered("No recording begins here. Change default devices in macOS System Settings > Sound.",0,buttons[1].maxY+42,w,12,gray(0.7))
    }
}

extension App {
    func beginAudioSession(enableInput:Bool){
        guard !tracker.audioSetupBusy else{return}
        tracker.audioSetupBusy=true;tracker.audioSetupMessage="";tracker.needsDisplay=true
        if enableInput && AVCaptureDevice.authorizationStatus(for:.audio) == .notDetermined {
            AVCaptureDevice.requestAccess(for:.audio){[weak self] _ in DispatchQueue.main.async{self?.finishAudioSession(enableInput:enableInput)}}
        }else{finishAudioSession(enableInput:enableInput)}
    }
    private func finishAudioSession(enableInput:Bool){
        defer{tracker.audioSetupBusy=false;tracker.refreshAudioSetup()}
        do{
            if !engine.isRunning{
                if source == nil {
                    let r=engine.outputNode.outputFormat(forBus:0).sampleRate,rate=r>0 ? r:48000
                    guard let format=AVAudioFormat(standardFormatWithSampleRate:rate,channels:2) else{tracker.audioSetupMessage="No compatible audio output. Select a device and retry.";return}
                    tm_init(rate)
                    source=AVAudioSourceNode(format:format){_,_,count,buffer -> OSStatus in
                        let b=UnsafeMutableAudioBufferListPointer(buffer)
                        if b.count>=2,let l=b[0].mData?.assumingMemoryBound(to:Float.self),let r=b[1].mData?.assumingMemoryBound(to:Float.self){tm_render(l,r,Int32(count))}
                        return noErr
                    }
                    engine.attach(source);engine.connect(source,to:engine.mainMixerNode,format:format)
                    tracker.loadedSlots=[];tracker.sync()
                }
                try engine.start()
            }
            tracker.audioEngineReady=true
            tracker.audioSetupVisible=false
            tracker.homeSelection=0
            tracker.message=enableInput && AVCaptureDevice.authorizationStatus(for:.audio) != .authorized ? "Audio ready • input permission unavailable" : "Audio ready"
            if let action=tracker.queuedHomeAction{tracker.queuedHomeAction=nil;DispatchQueue.main.async{[weak self] in self?.tracker.homeAction(action)}}
        }catch{tracker.audioEngineReady=false;tracker.audioSetupMessage="Audio engine could not start. Select output and retry."}
    }
}

func audioSetupSelfTest(){let t=Tracker(frame:NSRect(x:0,y:0,width:1440,height:900));t.home=true;t.openAudioSetup();var requested:Bool?;t.startAudioSession={requested=$0};precondition(t.audioSetupVisible);t.audioSetupActivate();precondition(requested==true);t.audioSetupChoice=1;t.audioSetupActivate();precondition(requested==false);t.audioSetupBusy=true;t.audioSetupActivate();precondition(requested==false);print("Audio setup start/skip routing passed")}
