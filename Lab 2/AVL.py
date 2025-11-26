from BST import BST, Node

class AVLNode(Node):
    def __init__(self, value):
        super().__init__(value)
        self.balance = 0

class Rotation:
    def left_rotation(self, a):
        b = a.right
        if b is None:
            return
        c = b.left
        b.parent = a.parent
        if a.parent is None:
            self.root = b
        elif a == a.parent.left:
            a.parent.left = b
        else:
            a.parent.right = b
        a.parent = b
        b.left = a
        if c is not None:
            c.parent = a
        a.right = c

    def right_rotation(self, a):
        b = a.left
        if b is None:
            return
        c = b.right
        b.parent = a.parent
        if a.parent is None:
            self.root = b
        elif a == a.parent.left:
            a.parent.left = b
        else:
            a.parent.right = b
        a.parent = b
        b.right = a
        if c is not None:
            c.parent = a
        a.left = c

class AVL(BST, Rotation):
    def _create_node(self, value) -> AVLNode:
        return AVLNode(value)

    def insert(self, value):
        x = super().insert(value)
        self._update_balances(x)
        self._balance(x)

    def delete_value(self, value):
        x = super().delete_value(value)
        self._update_balances(x)
        self._balance(x)

    def _balance(self, g):
        while g is not None: # node - дед
            if g.balance > 1: # случай 1 и 3
                left_balance = g.left.balance if not self._is_empty(g.left) else 0
                if left_balance > 0: # случай 1
                    self._small_right_rotation(g)
                else: # случай 3
                    self._big_right_rotation(g)
                self._update_balances(g)
            elif g.balance < -1: # случай 2 и 4
                right_balance = g.right.balance if not self._is_empty(g.right) else 0
                if right_balance < 0: # случай 2
                    self._small_left_rotation(g)
                else: # случай 4
                    self._big_left_rotation(g)
                self._update_balances(g)
            g = g.parent

    def _update_balances(self, p):
        while p is not None:
            left_height = self.height(p.left) if not self._is_empty(p.left) else 0
            right_height = self.height(p.right) if not self._is_empty(p.right) else 0
            p.balance = left_height - right_height
            p = p.parent
        
    def _small_left_rotation(self, a):
        self.left_rotation(a)

    def _small_right_rotation(self, a):
        self.right_rotation(a)

    def _big_left_rotation(self, node):
        self.right_rotation(node.right)
        self.left_rotation(node)

    def _big_right_rotation(self, node):
        self.left_rotation(node.left)
        self.right_rotation(node)



if __name__ == "__main__":
    Tree = AVL()
    import random
    unique_random_numbers = random.sample(range(-10, 26), 20)
    for n in unique_random_numbers:
        Tree.insert(n)
    print(Tree)
    print(f"Tree.height = {Tree.height()}")
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
        print(Tree)
        Tree.delete_value(value)
    print(Tree)

    # построение графика h(n), верхней и нижней оценки AVL дерева
    import matplotlib.pyplot as plt
    import math
    x = []
    y = []
    high = []
    low = []
    Tree = AVL()
    number_of_elements = 10000
    # рандомные значения
    # unique_random_numbers = random.sample(range(number_of_elements), number_of_elements)
    # for n in range(number_of_elements):
    #     x.append(n)
    #     Tree.insert(unique_random_numbers[n])
    #     y.append(Tree.height())
    #     high.append(1.44 * math.log2(n + 1) + 1.67)
    #     low.append(math.log2(n + 1))
    # plt.plot(x, y, color="blue", label="h(n)")
    # plt.plot(x, high, color="red", label="upper limit")
    # plt.plot(x, low, color="green", label="lower limit")
    # plt.xlabel("n")
    # plt.ylabel("h")
    # plt.legend()
    # plt.title("Зависимость высоты h AVL дерева от количества\nэлементов n(элементы генерируются случайным образом)")
    # plt.show()
    # возрастающие значения
    for n in range(number_of_elements):
        x.append(n)
        Tree.insert(n)
        y.append(Tree.height())
        high.append(1.44 * math.log2(n + 1) + 1.67)
        low.append(math.log2(n + 1))
    plt.plot(x, y, color="blue", label="h(n)")
    plt.plot(x, high, color="red", label="upper limit")
    plt.plot(x, low, color="green", label="lower limit")
    plt.xlabel("n")
    plt.ylabel("h")
    plt.legend()
    plt.title("Зависимость высоты h AVL дерева от количества\nэлементов n(ключи монотонно возрастают)")
    plt.show()