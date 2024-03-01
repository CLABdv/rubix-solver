#!/usr/bin/python3.11
import matplotlib.pyplot as plt

def parse_file_line(line):
    split_index = line.rfind('], ')
    coordinates_str = line[:split_index + 1]
    coordinates = eval(coordinates_str)
    remaining_parts = line[split_index + 3:].split(', ')
    value = float(remaining_parts[0])
    pos = remaining_parts[1]
    color = remaining_parts[2]
    return (list(map(lambda x: int(round(x * value)), coordinates)), color, int(pos)) #i want haskell back

f = open("log.data", "r", encoding="ascii")
read_data = list(map(parse_file_line, f.read().splitlines(keepends=False)))
f.close()

fig = plt.figure()
ax = fig.add_subplot(projection='3d')
ax.set_facecolor("gray")

for line in read_data:
    (xs, ys, zs) = line[0]
    m="o"
    if line[2] == 0: #corner
        m  = "D"
    elif line[2] == 1: #edge
        m = "X"
    elif line[2] == 2: #middle
        m = "*"
    col = line[1]
    if col == "orange":
        col = "black"
    ax.scatter(xs, ys, zs, c = col, marker=m)

ax.set_xlabel('Red')
ax.set_ylabel('Green')
ax.set_zlabel('Blue')

plt.show()
