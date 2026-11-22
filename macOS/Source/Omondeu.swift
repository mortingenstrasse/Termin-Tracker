import AppKit

struct OmondeuPatch: Codable, Equatable {
    var values:[Int] = [1,2,70,35,75,-1,0,7,800,28,35,45,4,250,75,180,0,70,60,22,350,0,0]
    static let labels=["Wave A","Wave B","Osc A Mix","Osc B Mix","Sub Mix","Sub Octave","B Semitone","Detune","Cutoff","Resonance","Filter Env","Drive","Attack","Decay","Sustain","Release","Voice Mode","Glide","Stereo Width","Pulse Width","Filter Decay","LFO Rate","LFO Depth"]
    static let ranges:[ClosedRange<Int>]=[0...3,0...3,0...100,0...100,0...100,-2...0,-24...24,0...40,30...16000,0...95,0...100,0...100,2...2000,10...3000,0...100,5...3000,0...2,0...1000,0...100,-45...45,10...3000,0...100,0...100]
    static let names=["Deep Analog","Pure Sub","Acid Pulse","Wide Reese","Velvet Keys","Slow Motion"]
    static func preset(_ i:Int)->OmondeuPatch {
        var p=OmondeuPatch()
        switch i {
        case 1:p.values=[0,0,35,0,100,-1,0,0,180,5,0,0,8,200,95,190,2,85,0,0,300,0,0]
        case 2:p.values=[2,1,82,30,40,-1,0,4,340,83,90,70,2,180,65,100,2,65,20,-15,210,0,0]
        case 3:p.values=[1,1,65,85,80,-1,0,18,1250,20,18,60,8,350,90,220,1,40,100,0,450,3,15]
        case 4:p.values=[3,0,85,40,15,-1,12,9,2400,15,22,14,18,500,65,500,0,0,75,0,500,0,0]
        case 5:p.values=[1,2,65,60,35,-1,7,12,520,48,35,25,180,900,85,850,0,0,95,12,950,4,60]
        default:break
        }
        return p
    }
    var valid:Bool{values.count==Self.ranges.count && zip(values,Self.ranges).allSatisfy{$0.1.contains($0.0)}}
    func publish(_ slot:Int){guard valid else{return};let a=values.map(Int32.init);a.withUnsafeBufferPointer{tm_omondeu(Int32(slot),$0.baseAddress)}}
    func valueText(_ i:Int)->String{let v=values[i];switch i{case 0,1:return ["Sine","Saw","Pulse","Triangle"][v];case 5,6:return String(format:"%+d",v);case 7:return "\(v) ct";case 8:return "\(v) Hz";case 12,13,15,17,20:return "\(v) ms";case 16:return ["Poly (8)","Mono","Legato"][v];case 19:return "\(50+Int(Double(v)*0.4))%";case 21:return String(format:"%.1f Hz",Double(v)/10);default:return "\(v)%"}}
}
extension Tracker {
    var omPatch:OmondeuPatch{slots[instrumentRow].omondeu ?? OmondeuPatch()}
    func openOmondeu(){
        if page==0{guard !addTrackSelected else{return};let i=cell.instrument ?? trackDefaults[track];guard i>=0 else{message="Assign Omondeu first";return};instrumentRow=i}
        guard slots[instrumentRow].wave==9,slots[instrumentRow].pcm==nil else{message="Select an Omondeu instrument (slot 10 or F2/F3)";return}
        omReturn=page;omFocus=0;omPreset=0;selectPage(12)
    }
    func setOmondeu(_ p:OmondeuPatch){guard p.valid,slots[instrumentRow].wave==9 else{return};checkpoint();var sl=slots;sl[instrumentRow].omondeu=p;project.slots=sl;syncInstruments();needsDisplay=true}
    func changeOmondeu(_ d:Int){var p=omPatch;let i=omFocus,r=OmondeuPatch.ranges[i],v=p.values[i];let step=i==8 ? max(5,v/20):[12,13,15,17,20].contains(i) ? 10:1;p.values[i]=min(r.upperBound,max(r.lowerBound,v+d*step));if p != omPatch{setOmondeu(p)}}
    func omPreview(){tm_preview(packed([max(0,min(127,(octave+1)*12))]),Int32(instrumentRow))}
    func omSoft(_ i:Int){switch i{case 0:omPreview();case 1,2:omPreset=(omPreset+(i==1 ? 5:1))%OmondeuPatch.names.count;setOmondeu(.preset(omPreset));message=OmondeuPatch.names[omPreset];case 3:omFocus=0;case 4:omFocus=8;case 5:omFocus=12;case 6:omFocus=16;case 7:selectPage(omReturn);default:break}}
    func omKey(_ e:NSEvent)->Bool{let c=e.charactersIgnoringModifiers?.lowercased() ?? "",d=e.modifierFlags.contains(.shift) ? 10:1;switch e.keyCode{case 53:selectPage(omReturn);case 126:omFocus=max(0,omFocus-1);case 125:omFocus=min(22,omFocus+1);case 123:omFocus=max(0,omFocus-8);case 124:omFocus=min(22,omFocus+8);case 36,76:omPreview();default:if c=="q"{changeOmondeu(-d)}else if c=="e"{changeOmondeu(d)}else if c=="u"{selectPage(omReturn)}else if c=="["{octave=max(0,octave-1)}else if c=="]"{octave=min(8,octave+1)}else{return false}};needsDisplay=true;return true}
    func omGeometry()->(CGFloat,CGFloat){((bounds.width-100)/3,min(54,max(24,(footerY-200)/8)))}
    func drawOmondeu(){let p=omPatch,(cw,rh)=omGeometry();txt("OMONDEU / ANALOG-INSPIRED SYNTH",50,66,12,teal,true);txt("SLOT \(instrumentRow+1)  •  \(slots[instrumentRow].name)  •  Stereo harmonic layer / centered sub",50,96,12,yellow);for (i,title) in ["OSCILLATORS / MIX","FILTER / ENVELOPE","VOICE / MODULATION"].enumerated(){txt(title,50+CGFloat(i)*cw,137,12,purple,true)};for i in p.values.indices{let col=i/8,x=50+CGFloat(col)*cw,y=158+CGFloat(i%8)*rh,r=OmondeuPatch.ranges[i];rect(x,y,cw-12,rh-4,i==omFocus ? gray(0.32):gray(0.19));if i==omFocus{border(x,y,cw-12,rh-4,teal)};txt(OmondeuPatch.labels[i],x+8,y+6,12,gray(0.85));txt(p.valueText(i),x+cw*0.56,y+6,12,i==omFocus ? teal:yellow,true);if rh>=35{let f=CGFloat(p.values[i]-r.lowerBound)/CGFloat(max(1,r.upperBound-r.lowerBound));rect(x+8,y+rh-12,cw-28,3,gray(0.32));rect(x+8,y+rh-12,(cw-28)*f,3,i==omFocus ? teal:purple)}};txt("↑↓ Select  •  ←→ Section  •  Q/E Value  •  Shift Fast  •  [ ] Octave  •  Enter Preview  •  Esc Back",50,footerY-28,12,gray(0.75))}
    func omMouse(_ pos:NSPoint)->Bool{let (cw,rh)=omGeometry();guard pos.x>=50,pos.x<50+3*cw,pos.y>=158,pos.y<158+8*rh else{return false};let i=Int((pos.x-50)/cw)*8+Int((pos.y-158)/rh);if i<23{omFocus=i;needsDisplay=true};return true}
}
func omondeuSelfTest(){let t=Tracker(frame:NSRect(x:0,y:0,width:1440,height:900));precondition(t.slots[9].name=="Omondeu");t.assignInstrument(9,wholeTrack:false);t.openOmondeu();precondition(t.page==12&&t.instrumentRow==9);t.omSoft(2);precondition(t.omPatch==OmondeuPatch.preset(1));t.omFocus=8;t.changeOmondeu(1);let project=try! JSONDecoder().decode(Project.self,from:JSONEncoder().encode(t.project));precondition(t.valid(project)&&project.slots?[9].omondeu==t.omPatch);t.omSoft(7);precondition(t.page==0);print("Omondeu assignment, controls and project roundtrip passed")}
