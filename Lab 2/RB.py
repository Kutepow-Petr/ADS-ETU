from BST import BST
from AVL import Rotation


BLACK = False
RED = True

class NIL:
    def __init__(self, parent):
        self.color = BLACK
        self.parent = parent
        self.value = None
        self.left = None
        self.right = None

    def __bool__(self):
        return False
    
    def __str__(self):
        return "NIL"

class RBNode:
    def __init__(self, value):
        self.value = value
        self.color = RED
        self.left = NIL(self)
        self.right = NIL(self)
        self.parent = None

    def __str__(self):
        return f"{self.value} {'●' if self.color == RED else '○'}"

class RB(BST, Rotation):
    def __init__(self):
        self.root = NIL(None)

    def _create_node(self, value) -> RBNode:
        return RBNode(value)
    
    def _is_empty(self, n) -> bool:
        return True if n is None else n.value is None
    
    def _create_empty_child(self, n):
        return NIL(n)
    
    def _copy_values(self, d, s):
        d.value = s.value
        d.color = s.color
    
    def insert(self, value):
        x = super().insert(value)
        self._balance_insert(x)

    def _balance_insert(self, x):
        if x.parent is None:
            x.color = BLACK
            return
        while x.parent is not None and x.parent.color == RED:
            p = x.parent
            g = p.parent
            if p == g.left:
                u = g.right
                if u.color == RED: # случай 1
                    p.color = BLACK
                    u.color = BLACK
                    g.color = RED
                    x = g
                else: # случай 2, 3
                    if x == p.right: # случай 2
                        x = p
                        self.left_rotation(x)
                    p = x.parent # случай 3
                    g = p.parent
                    p.color = BLACK
                    g.color = RED
                    self.right_rotation(g)
            else:
                u = g.left
                if u.color == RED: # случай 1
                    p.color = BLACK
                    u.color = BLACK
                    g.color = RED
                    x = g
                else: # случай 2, 3
                    if x == p.left: # случай 2
                        x = p
                        self.right_rotation(x)
                    p = x.parent # случай 3
                    g = p.parent
                    p.color = BLACK
                    g.color = RED
                    self.left_rotation(g)
        self.root.color = BLACK

    def delete_value(self, value):
        d = self.find(value)
        if d is not None:
            d_color = d.color
            x = super().delete_node(d)
            if d_color == BLACK:
                self._balance_delete(x)

    def _balance_delete(self, x):
        while x != self.root and x.color == BLACK:
            p = x.parent
            if x == p.left:
                b = p.right
                if b.value is None:
                    x = x.parent
                    continue
                if b.color == RED: # случай 2
                    b.color = BLACK
                    p.color = RED
                    self.left_rotation(p)
                    p = b
                    b = p.right
                    if b.value is None:
                        x = x.parent
                        continue
                nl = b.left
                nr = b.right
                if p.color == BLACK and b.color == BLACK and nl.color == BLACK and nr.color == BLACK: # случай 3
                    b.color = RED
                    continue
                if b.color == BLACK and nl.color == BLACK and nr.color == BLACK: # случай 4
                    b.color = RED
                    p.color = BLACK
                    break
                elif b.color == BLACK and nl.color == RED and nr.color == BLACK: # случай 5
                    nl.color = BLACK
                    b.color = RED
                    self.right_rotation(b)
                    b = p.right
                    nl = b.left
                    nr = b.right
                elif nr.color == RED: # случай 6
                    b.color = p.color
                    p.color = BLACK
                    nr.color = BLACK
                    self.left_rotation(p)
                    break
            else:
                b = p.left
                if b.value is None:
                    x = x.parent
                    continue
                if b.color == RED: # случай 2
                    b.color = BLACK
                    p.color = RED
                    self.right_rotation(p)
                    p = b
                    b = p.left
                    if b.value is None:
                        x = x.parent
                        continue
                nl = b.left
                nr = b.right
                if p.color == BLACK and b.color == BLACK and nl.color == BLACK and nr.color == BLACK: # случай 3
                    b.color = RED
                    continue
                if b.color == BLACK and nl.color == BLACK and nr.color == BLACK: # случай 4
                    b.color = RED
                    p.color = BLACK
                    break
                elif b.color == BLACK and nr.color == RED and nl.color == BLACK: # случай 5
                    nr.color = BLACK
                    b.color = RED
                    self.left_rotation(b)
                    b = p.left
                    nl = b.left
                    nr = b.right
                elif nl.color == RED: # случай 6
                    b.color = p.color
                    p.color = BLACK
                    nl.color = BLACK
                    self.right_rotation(p)
                    break
        self.root.color = BLACK # Случай 1



if __name__ == "__main__":
    Tree = RB()
    import random
    unique_random_numbers = random.sample(range(-10, 26), 20)
    for i in unique_random_numbers:
        Tree.insert(i)
    print(Tree)
    found_node = Tree.find(random.choice(unique_random_numbers))
    print(f'''Node.value = {found_node.value};
    el.parent: {found_node.parent},
    el.left_child: {found_node.left},
    el.right_child: {found_node.right}.''')
    found_node = Tree.find(-1000)
    print(found_node)
    print("Min =", Tree.min())
    print("Max =", Tree.max())
    print("Sorted =", Tree.in_order(Tree.root))
    print("Structure =", Tree.pre_order(Tree.root))
    print("Deletion =", Tree.post_order(Tree.root))
    print("Levels =", Tree.level_order())
    deleted_nodes = random.sample(unique_random_numbers, 3)
    print(f"Deleted nodes = {deleted_nodes}")
    for value in deleted_nodes:
        Tree.delete_value(value)
    print(Tree)

    # построение графика h(n), верхней и нижней оценки RB дерева
    import matplotlib.pyplot as plt
    import math
    x = []
    y = []
    high = []
    low = []
    Tree = RB()
    number_of_elements = 10000
    # рандомные значения
    unique_random_numbers = random.sample(range(number_of_elements), number_of_elements)
    for n in range(number_of_elements):
        x.append(n)
        Tree.insert(unique_random_numbers[n])
        y.append(Tree.height())
        high.append(2 * math.log2(n + 1))
        low.append(math.log2(n + 1))
    plt.plot(x, y, color="blue", label="h(n)")
    plt.plot(x, high, color="red", label="upper limit")
    plt.plot(x, low, color="green", label="lower limit")
    plt.xlabel("n")
    plt.ylabel("h")
    plt.legend()
    plt.title("Зависимость высоты h RB дерева от количества\nэлементов n(элементы генерируются случайным образом)")
    plt.show()
    # возрастающие значения
    for n in range(number_of_elements):
        x.append(n)
        Tree.insert(n)
        y.append(Tree.height())
        high.append(2 * math.log2(n + 1))
        low.append(math.log2(n + 1))
    plt.plot(x, y, color="blue", label="h(n)")
    plt.plot(x, high, color="red", label="upper limit")
    plt.plot(x, low, color="green", label="lower limit")
    plt.xlabel("n")
    plt.ylabel("h")
    plt.legend()
    plt.title("Зависимость высоты h RB дерева от количества\nэлементов n(ключи монотонно возрастают)")

    plt.show()
