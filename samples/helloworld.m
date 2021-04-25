# main () = printf "hello world !\n"

type Point2D = x:double y:double
f xy:Point2D = xy.y
main() = 
  xy:Point2D = 10.0 20.0
  result = f xy
  printf "call pass by big struct: %f\n" result


