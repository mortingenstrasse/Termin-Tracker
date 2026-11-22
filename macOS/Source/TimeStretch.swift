import AVFoundation

// Offline Rubber Band R3. The command-line tool is a separate process, not linked into TrackerMac.
enum TimeStretch {
    static var executable:URL?{["/opt/homebrew/bin/rubberband","/usr/local/bin/rubberband"].first(where:{FileManager.default.isExecutableFile(atPath:$0)}).map{URL(fileURLWithPath:$0)}}
    static func error(_ message:String)->NSError{NSError(domain:"TimeStretch",code:1,userInfo:[NSLocalizedDescriptionKey:message])}
    static func render(_ pcm:[Float],rate:Double,percent:Int)throws->[Float]{
        guard !pcm.isEmpty,(50...200).contains(percent),rate>=8000,rate<=192000 else{throw error("Invalid stretch range")}
        if percent==100{return pcm}
        let frames=max(1,Int((Double(pcm.count)*Double(percent)/100).rounded()))
        guard frames<=1_000_000 else{throw error("Stretch exceeds the 1,000,000-frame slot limit")}
        guard let executable=executable else{throw error("Rubber Band R3 unavailable. Install with: brew install rubberband")}
        let fm=FileManager.default,dir=fm.temporaryDirectory.appendingPathComponent("TrackerStretch-\(UUID().uuidString)",isDirectory:true)
        try fm.createDirectory(at:dir,withIntermediateDirectories:false);defer{try? fm.removeItem(at:dir)}
        let input=dir.appendingPathComponent("input.wav"),output=dir.appendingPathComponent("output.wav"),format=AVAudioFormat(standardFormatWithSampleRate:rate,channels:1)!,buffer=AVAudioPCMBuffer(pcmFormat:format,frameCapacity:AVAudioFrameCount(pcm.count))!
        buffer.frameLength=AVAudioFrameCount(pcm.count);pcm.withUnsafeBufferPointer{buffer.floatChannelData![0].update(from:$0.baseAddress!,count:pcm.count)}
        do{let file=try AVAudioFile(forWriting:input,settings:format.settings,commonFormat:.pcmFormatFloat32,interleaved:false);try file.write(from:buffer)}
        let proc=Process();proc.executableURL=executable;proc.arguments=["-3","-q","-t",String(Double(percent)/100),input.path,output.path];proc.standardOutput=FileHandle.nullDevice;proc.standardError=FileHandle.nullDevice;try proc.run();proc.waitUntilExit();guard proc.terminationStatus==0 else{throw error("Rubber Band failed (\(proc.terminationStatus))")}
        let file=try AVAudioFile(forReading:output,commonFormat:.pcmFormatFloat32,interleaved:false);guard abs(file.fileFormat.sampleRate-rate)<1,file.fileFormat.channelCount==1,file.length>0,file.length<=1_010_000 else{throw error("Invalid stretch output")};let result=AVAudioPCMBuffer(pcmFormat:file.processingFormat,frameCapacity:AVAudioFrameCount(file.length))!;try file.read(into:result);var out=Array(UnsafeBufferPointer(start:result.floatChannelData![0],count:Int(result.frameLength)));guard abs(out.count-frames)<=max(Int(rate*0.05),Int(Double(frames)*0.03)) else{throw error("Unexpected stretch duration: \(out.count) vs \(frames)")};if out.count>frames{out.removeLast(out.count-frames)};if out.count<frames{let fade=min(out.count,max(1,Int(rate*0.003)));for i in 0..<fade{out[out.count-fade+i]*=Float(fade-1-i)/Float(fade)};out.append(contentsOf:repeatElement(Float(0),count:frames-out.count))};guard out.allSatisfy({$0.isFinite}) else{throw error("Invalid samples in stretch output")};let peak=out.reduce(Float(0)){max($0,abs($1))};if peak>1{out=out.map{$0/peak}};return out
    }
}

func timeStretchSelfTest() throws {
    guard TimeStretch.executable != nil else{print("Rubber Band R3 not installed; skipping offline stretch test");return}
    let rate=48000.0,pcm=(0..<48000).map{Float(sin(2*Double.pi*440*Double($0)/rate)*0.25)}
    for percent in [50,150,200]{let x=try TimeStretch.render(pcm,rate:rate,percent:percent);precondition(x.count==48000*percent/100);let lo=x.count/4,hi=x.count*3/4;var crossings=0,energy:Double=0;for i in lo+1..<hi{if x[i-1]<0&&x[i]>=0{crossings+=1};energy+=Double(x[i]*x[i])};let hz=Double(crossings)*rate/Double(hi-lo);print("Time stretch \(percent)%: \(x.count) frames, pitch \(hz) Hz, RMS \(sqrt(energy/Double(hi-lo)))");precondition(abs(hz-440)<8&&energy/Double(hi-lo)>0.005)}
}
