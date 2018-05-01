object BSTSet
  sealed abstract class Tree
  private case class Leaf() extends Tree
  private case class Node(left: Tree, value : Int, right: Tree) extends Tree
    @invariant(max(content(left)) < value && value < min(content(right)))
  
  def content(t: Tree): Set[int] t match {
    case Leaf() => {}
    case Node(l,e,r) => content(l) + {e} + content(r)
  }

  def find(e: Int, t : Tree): Boolean = (t match {
    case Leaf() => false
    case Node(l,v,r) =>
      if(e < v) find(e, l)
      else if (e == v) true
      else find(e, r)
  }ensuring (res => (res == (e in content(t))))
