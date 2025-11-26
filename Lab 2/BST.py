class Node:
    def __init__(self, value):
        self.value = value
        self.left = None
        self.right = None
        self.parent = None

    def __str__(self) -> str:
        return str(self.value)

class BST:
    def __init__(self):
        self.root = self._create_empty_child(None)

    def __str__(self, с=None, level=1) -> str:
        res = ""
        if с is None:
            с = self.root
            res += "Root: "
        if с.left is None and с.right is None:
            res += str(с.__str__())
        elif с.left is not None and с.right is None:
            res += str(с.__str__())
            res += "\n" + level * "|   " + "L: " + self.__str__(с.left, level + 1)
            res += "\n" + level * "|   " + "R: None"
        elif с.left is None and с.right is not None:
            res += str(с.__str__())
            res += "\n" + level * "|   " + "L: None"
            res += "\n" + level * "|   " + "R: " + self.__str__(с.right, level + 1)
        else:
            res += str(с.__str__())
            res += "\n" + level * "|   " + "L: " + self.__str__(с.left, level + 1)
            res += "\n" + level * "|   " + "R: " + self.__str__(с.right, level + 1)
        return res
    
    def _create_node(self, value) -> Node:
        return Node(value)
    
    def _is_empty(self, n) -> bool:
        return n is None
    
    def _create_empty_child(self, n):
        return None
    
    def _copy_values(self, d, s):
        d.value = s.value

    def insert(self, value):
        # c - текущая нода, x - новая, p - родитель для x
        c = self.root
        x = self._create_node(value)
        if self._is_empty(c):
            self.root = x
        else:
            p = None
            while not self._is_empty(c):
                p = c
                if c.value < value:
                    c = c.right
                else:
                    c = c.left
            x.parent = p
            if p.value < value:
                p.right = x
            else:
                p.left = x
        return x
    
    def find(self, value):
        n = self.root
        while not self._is_empty(n):
            if n.value == value:
                return n
            if n.value < value:
                n = n.right
            else:
                n = n.left
        return None
    
    def min(self):
        n = self.root
        while not self._is_empty(n.left):
            n = n.left
        return n
    
    def max(self):
        n = self.root
        while not self._is_empty(n.right):
            n = n.right
        return n
    
    def delete_value(self, value):
        # d - к удалению, p - родитель удалённой
        d = self.find(value)
        x = None
        if d is not None:
            x = self.delete_node(d)
        return x
    
    def delete_node(self, d):
        # d - к удалению, p - родитель d, s - преемник
        x = None
        p = d.parent
        if self._is_empty(d.left) and self._is_empty(d.right): # нет потомков
            if not self._is_empty(p):
                x = self._create_empty_child(p)
                if p.value < d.value:
                    p.right = x
                else:
                    p.left = x
            else:
                x = self._create_empty_child(None)
                self.root = x
            d = None

        elif not self._is_empty(d.left) and self._is_empty(d.right): # только левый потомок
            x = d.left
            x.parent = p
            if self._is_empty(p):
                self.root = x
            elif p.left == d:
                p.left = x
            else:
                p.right = x
            d = None

        elif self._is_empty(d.left) and not self._is_empty(d.right): # только правый потомок
            x = d.right
            x.parent = p
            if self._is_empty(p):
                self.root = x
            elif p.left == d:
                p.left = x
            else:
                p.right = x
            d = None

        else: # оба потомка
            s = d.right
            while not self._is_empty(s.left):
                s = s.left
            self._copy_values(d, s)
            x = self.delete_node(s)
        return x

    def height(self, c=None) -> int:
        if c is None:
            c = self.root
            if self._is_empty(c):
                return 0
        left_height = self.height(c.left) if not self._is_empty(c.left) else 0
        right_height = self.height(c.right) if not self._is_empty(c.right) else 0
        return max(left_height, right_height) + 1
    
    def in_order(self, n):
        res = []
        if not self._is_empty(n):
            res.extend(self.in_order(n.left))
            res.extend([n.value,])
            res.extend(self.in_order(n.right))
        return res
    
    def pre_order(self, n):
        res = []
        if not self._is_empty(n):
            res.extend([n.value,])
            res.extend(self.pre_order(n.left))
            res.extend(self.pre_order(n.right))
        return res
    
    def post_order(self, n):
        res = []
        if not self._is_empty(n):
            res.extend(self.post_order(n.left))
            res.extend(self.post_order(n.right))
            res.extend([n.value,])
        return res
    
    def level_order(self):
        from collections import deque
        if self._is_empty(self.root):
            return
        queue = deque([self.root])
        res = []
        while queue:
            n = queue.popleft()
            res.append(n.value)
            if not self._is_empty(n.left):
                queue.append(n.left)
            if not self._is_empty(n.right):
                queue.append(n.right)
        return res



if __name__ == "__main__":
    Tree = BST()
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
        Tree.delete_value(value)
    print(Tree)

    # построение графика h(n)
    import matplotlib.pyplot as plt
    x = []
    y = []
    Tree = BST()
    number_of_elements = 10000
    unique_random_numbers = random.sample(range(number_of_elements), number_of_elements)
    for n in range(number_of_elements):
        x.append(n)
        Tree.insert(unique_random_numbers[n])
        y.append(Tree.height())
    plt.plot(x, y, label="h(n)")
    plt.xlabel("n")
    plt.ylabel("h")
    plt.legend()
    plt.title("Зависимость высоты h от количества элементов n\nв BST (элементы генерируются случайным образом)")
    plt.show()