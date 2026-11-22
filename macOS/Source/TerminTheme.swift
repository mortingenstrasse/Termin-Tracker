import AppKit

func terminHex(_ rgb: UInt32) -> NSColor {
    NSColor(srgbRed:CGFloat((rgb>>16)&255)/255,green:CGFloat((rgb>>8)&255)/255,blue:CGFloat(rgb&255)/255,alpha:1)
}
let terminBlack=terminHex(0x000000),terminPanel=terminHex(0x131313),terminBeat=terminHex(0x222121),terminLine=terminHex(0x515151)
let terminGreen=terminHex(0x546D54),terminCyan=terminHex(0x598E8E),terminOlive=terminHex(0x6C7448),terminText=terminHex(0xC0C0C0)
let terminPlayhead=terminHex(0x202A1F),terminRecord=terminHex(0x895D58),terminPlum=terminHex(0x7A6C7D)

// Retain the existing hierarchy of every panel while converting the old gray scale
// to the supplied palette. Labels are capped at the reference's soft off-white.
func terminGray(_ input:CGFloat)->NSColor{
    let stops:[(CGFloat,UInt32)]=[(0,0x000000),(0.09,0x090909),(0.15,0x131313),(0.20,0x131313),(0.26,0x222121),(0.34,0x303030),(0.45,0x515151),(0.55,0x6D6D6D),(0.65,0x8D8D8D),(0.80,0xC0C0C0),(1,0xC0C0C0)]
    let v=max(0,min(1,input))
    let i=stops.firstIndex(where:{$0.0>=v}) ?? stops.count-1
    if i==0{return terminHex(stops[0].1)}
    let a=stops[i-1],b=stops[i],f=(v-a.0)/(b.0-a.0)
    func component(_ shift:Int)->CGFloat{let x=CGFloat((a.1>>shift)&255),y=CGFloat((b.1>>shift)&255);return (x+(y-x)*f)/255}
    return NSColor(srgbRed:component(16),green:component(8),blue:component(0),alpha:1)
}

struct TerminIntro {
    static let duration=2.0
    var start:Double
    var seed:UInt64
    func elapsed(_ now:Double)->Double{max(0,min(Self.duration,now-start))}
    func finished(_ now:Double)->Bool{now-start>=Self.duration}
    func random(_ lane:Int,_ row:Int,_ field:Int)->UInt64{
        var x=seed &+ UInt64(truncatingIfNeeded:lane &* 9277 &+ row &* 433 &+ field &* 17)
        x ^= x>>30;x &*= 0xbf58476d1ce4e5b9;x ^= x>>27;x &*= 0x94d049bb133111eb;return x ^ (x>>31)
    }
}

extension Tracker {
    func beginTerminIntro(){intro=TerminIntro(start:ProcessInfo.processInfo.systemUptime,seed:UInt64.random(in:1...UInt64.max));home=true;homeSelection=0;needsDisplay=true}
    func tickTerminIntro(now:Double=ProcessInfo.processInfo.systemUptime){guard let current=intro else{return};if current.finished(now){intro=nil};needsDisplay=true}
    func homeGeometry()->(CGFloat,CGFloat,CGFloat){let w=min(320,bounds.width-40),start=max(116,min(bounds.height*0.39,bounds.height-CGFloat(homeItems.count)*50-77));return ((bounds.width-w)/2,w,start)}
    func drawTerminTitle(_ y:CGFloat,alpha:CGFloat=1){
        let s="TERMIN TRACKER" as NSString
        let font=NSFont.monospacedSystemFont(ofSize:min(32,max(20,bounds.width/46)),weight:.semibold)
        let attrs:[NSAttributedString.Key:Any]=[.font:font,.foregroundColor:terminText.withAlphaComponent(alpha),.kern:CGFloat(3.2)]
        let sw=s.size(withAttributes:attrs).width
        s.draw(at:NSPoint(x:(bounds.width-sw)/2,y:y),withAttributes:attrs)
        rect((bounds.width-54)/2,y+font.pointSize+10,54,1,terminCyan.withAlphaComponent(alpha))
    }
    func drawTerminHome(){
        let w=bounds.width,h=bounds.height,(x,bw,start)=homeGeometry()
        rect(0,0,w,h,terminBlack);rect(8,8,28,28,terminPanel);centered("×",8,12,28,12,gray(0.64),true)
        drawTerminTitle(start-87)
        for (i,item) in homeItems.enumerated(){
            let y=start+CGFloat(i)*50,selected=i==homeSelection
            rect(x,y,bw,42,selected ? terminPlayhead:terminPanel)
            border(x,y,bw,42,selected ? terminGreen:terminBeat)
            centered(item.uppercased(),x,y+14,bw,12,selected ? terminText:gray(0.70),selected)
        }
        let bottom=start+CGFloat(homeItems.count)*50
        centered("↑ ↓  SELECT    ENTER  OPEN",0,bottom+12,w,11,gray(0.62))
        centered(audioEngineReady ? "AUDIO READY  /  INPUT: "+microphoneStatus:"AUDIO NOT STARTED  /  SELECT START AUDIO",0,bottom+34,w,10,audioEngineReady ? terminCyan:terminOlive)
        centered("0.19",0,h-27,w,10,gray(0.45))
    }
    func drawTerminIntro(_ state:TerminIntro,at now:Double){
        let w=bounds.width,h=bounds.height,t=state.elapsed(now)
        rect(0,0,w,h,terminBlack)
        let laneCount=max(3,min(10,Int(w/150))),laneW=w/CGFloat(laneCount),rowH:CGFloat=21
        let dissipate=CGFloat(max(0,min(1,(2.0-t)/0.48)))
        for lane in 0..<laneCount {
            let offset=Double(state.random(lane,0,0)%35)/100.0
            let velocity=h*CGFloat(0.72+Double(state.random(lane,0,1)%65)/100.0)
            let head=CGFloat(t-offset)*velocity
            if head<0{continue}
            let rowIndex=Int(head/rowH),phase=head-CGFloat(rowIndex)*rowH
            let trail=15+Int(state.random(lane,0,2)%12)
            for j in 0..<trail{
                let row=rowIndex-j,y=CGFloat(row)*rowH+phase
                if y<0||y>h{continue}
                let density=state.random(lane,row,3)%100
                if density<22&&j>0{continue}
                let a=CGFloat(max(0,1-Double(j)/Double(trail)))*dissipate
                let pitch=Int(state.random(lane,row,4)%48)+36,ins=1+Int(state.random(lane,row,5)%18),vel=35+Int(state.random(lane,row,6)%66)
                let x=CGFloat(lane)*laneW+5
                if j==0{rect(x,y,laneW-10,rowH-1,terminGreen.withAlphaComponent(a*0.20))}
                let note=noteName(pitch),fx=state.random(lane,row,7)%3==0 ? "v"+String(format:"%02d",state.random(lane,row,8)%90):"--"
                txt(String(format:"%02d",(row%100+100)%100),x+2,y+4,10,terminLine.withAlphaComponent(a))
                txt(note,x+25,y+3,12,terminText.withAlphaComponent(a))
                txt(String(format:"%02d %02d",ins,vel),x+58,y+3,11,terminOlive.withAlphaComponent(a))
                txt(fx,x+105,y+3,11,terminCyan.withAlphaComponent(a))
            }
        }
        if t>=1.38{let alpha=CGFloat(min(1,(t-1.38)/0.55));let (_,_,start)=homeGeometry();drawTerminTitle(start-87,alpha:alpha)}
    }
}

func terminThemeSelfTest(){
    let t=Tracker(frame:NSRect(x:0,y:0,width:1440,height:900))
    t.home=true;t.startAudioSession={_ in};precondition(t.homeItems.first=="Start Audio")
    t.homeAction("New");precondition(t.audioSetupVisible&&t.queuedHomeAction=="New")
    t.audioSetupVisible=false;t.queuedHomeAction=nil
    t.intro=TerminIntro(start:100,seed:6);t.tickTerminIntro(now:101.99);precondition(t.intro != nil);t.tickTerminIntro(now:102);precondition(t.intro==nil&&t.home)
    precondition(terminHex(0x000000)==trackerBackground&&fieldColors[0]==terminText&&fieldColors[1]==terminOlive&&fieldColors[4]==terminGreen&&fieldColors[5]==terminCyan)
    print("Termin theme, two-second intro and audio setup menu routing passed")
}
