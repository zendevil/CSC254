def tree(n):
    if n == 1:
        return ["(.)"]
    strings = []
    for T in tree(n-1):
        strings.append("(" + T + ".)")
        strings.append("(." + T + ")")
    return strings

print("Enter number of nodes: ")
for tree in tree(int(input())):
    print(tree)
