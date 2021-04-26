_fltused:int = 0
type Point2DD = x:double y:double
type Point2DI = x:int y:int

passByRef xy:Point2DD = xy.y
passByI64 xy:Point2DI = xy.y
returnSmallStruct() = 
  xy:Point2DI = 20 30
  xy
returnLargeStruct() = 
  xy:Point2DD = 200.0 300.0
  xy

main() = 
  printf "hello world !\n"
  xy_d:Point2DD = 10.0 40.0
  xy_i:Point2DI = 20 80
  result1 = passByRef xy_d
  result2 = passByI64 xy_i
  result3 = returnSmallStruct()
  result4 = returnLargeStruct()
  result5 = result3.x
  result6 = result4.x
  printf "call pass by large struct: %f\n" result1
  printf "call pass by small struct: %d\n" result2
  printf "return small struct: %d\n" result5
  printf "return large struct: %f\n" result6
  0


