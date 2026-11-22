import AppKit

extension Pattern {
    func heldOnset(track:Int,row:Int)->Int?{guard row>0,track>=0,track<cells.count,row<length else{return nil};for i in stride(from:row-1,through:0,by:-1){let c=cells[track][i];if c.noteOff==true{return nil};if !c.notes.isEmpty{let end=i*4+(c.micro ?? 0)/25+(c.duration ?? 0);return end>row*4 && (c.duration ?? 0)>0 ? i:nil}};return nil}
    func heldCell(track:Int,row:Int)->Cell?{guard cells[track][row].notes.isEmpty,cells[track][row].noteOff != true,let onset=heldOnset(track:track,row:row) else{return nil};return cells[track][onset]}
}
extension Tracker {
    func openDuration(){guard track<activeTrackCount else{return};let p=pattern;if !p.cells[track][row].notes.isEmpty{durationRow=row}else if let start=p.heldOnset(track:track,row:row){durationRow=start}else{message="Select a note first";return};durationEdit=p.cells[track][durationRow].duration ?? 0;modal=9;needsDisplay=true}
    func durationKey(_ e:NSEvent){let k=e.keyCode,ch=e.charactersIgnoringModifiers?.lowercased() ?? "",d=e.modifierFlags.contains(.shift) ? 4:1;switch k{case 53:modal=0;case 36,76:checkpoint();var p=pattern;p.cells[track][durationRow].duration=durationEdit==0 ? nil:durationEdit;pattern=p;push(project.selected,track,durationRow,p.cells[track][durationRow]);modal=0;case 51:durationEdit=0;default:if [124,126].contains(k)||ch=="e"{durationEdit=min(511,(pattern.length-durationRow)*4-(pattern.cells[track][durationRow].micro ?? 0)/25,durationEdit+d)}else if [123,125].contains(k)||ch=="q"{durationEdit=max(0,durationEdit-d)}};needsDisplay=true}
    func drawDuration(){let w:CGFloat=450,h:CGFloat=220,x=(bounds.width-w)/2,y=(bounds.height-h)/2;rect(0,0,bounds.width,bounds.height,.black.withAlphaComponent(0.6));rect(x,y,w,h,trackerBackground);border(x,y,w,h,teal);txt("NOTE DURATION",x+16,y+14,14,teal,true);centered(durationEdit==0 ? "Legacy / natural":String(format:"%.2f steps",Double(durationEdit)/4),x,y+72,w,28,yellow,true);centered("Q/E ¼ step • Shift ±1 step • Enter Apply • Esc",x,y+h-35,w,11,gray(0.8))}
}
