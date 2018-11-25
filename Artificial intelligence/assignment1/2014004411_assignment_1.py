import queue

def abs_sign(a):
  if a >=0:
    return a
  else:
    return -a

def find_index(info, table):
  start = (-1,-1)
  end = (-1,-1)
  key = (-1, -1)
  for i in range(0, int(info[1])):
    for j in range(0, int(info[2])):
      if(table[i][j] == '3'):
        start = (i,j)
      if(table[i][j] == '4'):
        end = (i,j)
      if(table[i][j] == '6'):
        key = (i,j)
  return start, end, key

def bfs(map, start, key, end):
  que = queue.Queue()
  time = 0
  route = []
  table = [[map[j][i] for i in range(len(map[0]))] for j in range(len(map))]
  current = start
  while current != key:
    if current[1] != len(table[0]) - 1 and (int(table[current[0]][current[1] + 1]) == 2 or int(table[current[0]][current[1] + 1]) ==6): #right
      que.put([(current[0], current[1] + 1)] + route)
    if current[0] != len(table) - 1 and (int(table[current[0] + 1][current[1]]) == 2 or int(table[current[0] + 1][current[1]]) == 6): #down
      que.put([(current[0]+1, current[1])] + route)
    if current[1] != 0 and (int(table[current[0]][current[1] - 1]) == 2 or int(table[current[0]][current[1] - 1]) == 6):  #left
      que.put([(current[0], current[1] - 1)] + route)
    if current[0] != 0 and (int(table[current[0] - 1][current[1]]) == 2 or int(table[current[0] - 1][current[1]]) == 6): #up
      que.put([(current[0] - 1, current[1])] + route)

    time += 1
    route = que.get()
    current = route[0]
    table[current[0]][current[1]] = 1

  que = queue.Queue()
  table = [[map[j][i] for i in range(len(map[0]))] for j in range(len(map))]

  while current != end:
    if current[1] != len(table[0]) - 1 and (int(table[current[0]][current[1] + 1]) == 2 or int(table[current[0]][current[1] + 1]) ==4): #right
      que.put([(current[0], current[1] + 1)] + route)
    if current[0] != len(table) - 1 and (int(table[current[0] + 1][current[1]]) == 2 or int(table[current[0] + 1][current[1]]) == 4): #down
      que.put([(current[0]+1, current[1])] + route)
    if current[1] != 0 and (int(table[current[0]][current[1] - 1]) == 2 or int(table[current[0]][current[1] - 1]) == 4):  #left
      que.put([(current[0], current[1] - 1)] + route)
    if current[0] != 0 and (int(table[current[0] - 1][current[1]]) == 2 or int(table[current[0] - 1][current[1]]) == 4): #up
      que.put([(current[0] - 1, current[1])] + route)

    time += 1
    route = que.get()
    current = route[0]
    table[current[0]][current[1]] = 1

  return time, route


def dfs(map, start, key, end):
  stack = queue.LifoQueue()
  time = 0
  route = []
  table = [[map[j][i] for i in range(len(map[0]))] for j in range(len(map))]
  current = start

  while current != key:
    if current[0] != 0 and (int(table[current[0] - 1][current[1]]) == 2 or int(table[current[0] - 1][current[1]]) ==  6): #up
      stack.put([(current[0] - 1, current[1])] + route)
    if current[1] != 0 and (int(table[current[0]][current[1] - 1]) == 2 or int(table[current[0]][current[1] - 1]) == 6): #left
      stack.put([(current[0], current[1] - 1)] + route)
    if current[0] != len(table) - 1 and (int(table[current[0] + 1][current[1]]) == 2 or int(table[current[0] + 1][current[1]]) == 6): #down
      stack.put([(current[0] + 1, current[1])] + route)
    if current[1] != len(table[0]) - 1 and (int(table[current[0]][current[1] + 1]) == 2 or int(table[current[0]][current[1] + 1]) == 6): #right
      stack.put([(current[0], current[1] + 1)] + route)

    route = stack.get()
    current = route[0]
    table[current[0]][current[1]] = 1
    time += 1

  stack = queue.LifoQueue()
  table = [[map[j][i] for i in range(len(map[0]))] for j in range(len(map))]

  while current != end:
    if current[0] != 0 and (int(table[current[0] - 1][current[1]]) == 2 or int(table[current[0] - 1][current[1]]) ==  4): #up
      stack.put([(current[0] - 1, current[1])] + route)
    if current[1] != 0 and (int(table[current[0]][current[1] - 1]) == 2 or int(table[current[0]][current[1] - 1]) == 4): #left
      stack.put([(current[0], current[1] - 1)] + route)
    if current[0] != len(table) - 1 and (int(table[current[0] + 1][current[1]]) == 2 or int(table[current[0] + 1][current[1]]) == 4): #down
      stack.put([(current[0] + 1, current[1])] + route)
    if current[1] != len(table[0]) - 1 and (int(table[current[0]][current[1] + 1]) == 2 or int(table[current[0]][current[1] + 1]) == 4): #right
      stack.put([(current[0], current[1] + 1)] + route)

    route = stack.get()
    current = route[0]
    table[current[0]][current[1]] = 1
    time += 1

  return time, route

def greedy(map, start, key, end):
  que = queue.PriorityQueue()
  time = 0
  route = []
  table = [[map[j][i] for i in range(len(map[0]))] for j in range(len(map))]
  que.put((abs_sign(key[0] - start[0]) + abs_sign(key[1] - start[1]), [start] + route))
  current = que.get()[1][0]

  while  current != key:
    if current[0] != (len(table) - 1) and (int(table[current[0] + 1][current[1]]) == 2 or int(table[current[0] + 1][current[1]]) == 6):
      que.put(( abs_sign(key[0] - (current[0] + 1)) + abs_sign(key[1] - current[1]), [(current[0] + 1, current[1])] + route))
    if current[1] != len(table[0]) - 1 and (int(table[current[0]][current[1] + 1]) == 2 or int(table[current[0]][current[1] + 1]) == 6):
      que.put((abs_sign(key[0] - current[0]) + abs_sign(key[1] - (current[1] + 1)), [(current[0], current[1] + 1)] + route))
    if current[1] != 0 and (int(table[current[0]][current[1] - 1]) == 2 or int(table[current[0]][current[1] - 1]) == 6):
      que.put((abs_sign(key[0] - current[0]) + abs_sign(key[1] - (current[1] - 1)), [(current[0], current[1] - 1)] + route))
    if current[0] != 0 and (int(table[current[0] - 1][current[1]]) == 2 or int(table[current[0] - 1][current[1]]) ==  6):
      que.put((abs_sign(key[0] -  (current[0] - 1)) + abs_sign(key[1] - current[1]), [(current[0] - 1, current[1])] + route))

    route = que.get()[1]
    current = route[0]
    table[current[0]][current[1]] = 1
    time = time + 1

  que = queue.PriorityQueue()
  table = [[map[j][i] for i in range(len(map[0]))] for j in range(len(map))]

  while current != end:
    if current[0] != (len(table) - 1) and (int(table[current[0] + 1][current[1]]) == 2 or int(table[current[0] + 1][current[1]]) == 4):
      que.put(( abs_sign(end[0] - (current[0] + 1)) + abs_sign(end[1] - current[1]), [(current[0] + 1, current[1])] + route))
    if current[1] != len(table[0]) - 1 and (int(table[current[0]][current[1] + 1]) == 2 or int(table[current[0]][current[1] + 1]) == 4):
      que.put((abs_sign(end[0] - current[0]) + abs_sign(end[1] - (current[1] + 1)), [(current[0], current[1] + 1)] + route))
    if current[1] != 0 and (int(table[current[0]][current[1] - 1]) == 2 or int(table[current[0]][current[1] - 1]) == 4):
      que.put((abs_sign(end[0] - current[0]) + abs_sign(end[1] - (current[1] - 1)), [(current[0], current[1] - 1)] + route))
    if current[0] != 0 and (int(table[current[0] - 1][current[1]]) == 2 or int(table[current[0] - 1][current[1]]) ==  4):
      que.put((abs_sign(end[0] -  (current[0] - 1)) + abs_sign(end[1] - current[1]), [(current[0] - 1, current[1])] + route))

    route = que.get()[1]
    current = route[0]
    table[current[0]][current[1]] = 1
    time += 1

  return time, route

def a_star(map, start, key, end):
  que = queue.PriorityQueue()
  time = 0
  route = []
  table = [[map[j][i] for i in range(len(map[0]))] for j in range(len(map))]
  distance = [[0 for i in range(len(map[0]))] for j in range(len(map))]
  que.put((abs_sign(key[0] - start[0]) + abs_sign(key[1] - start[1]), [start] + route))
  current = que.get()[1][0]

  while  current != key:
    if current[0] != (len(table) - 1) and (int(table[current[0] + 1][current[1]]) == 2 or int(table[current[0] + 1][current[1]]) == 6):
      distance[current[0] + 1][current[1]] = distance[current[0]][current[1]] + 1
      que.put(( abs_sign(key[0] - (current[0] + 1)) + abs_sign(key[1] - current[1]) + distance[current[0] + 1][current[1]], [(current[0] + 1, current[1])] + route))
    if current[1] != len(table[0]) - 1 and (int(table[current[0]][current[1] + 1]) == 2 or int(table[current[0]][current[1] + 1]) == 6):
      distance[current[0]][current[1] + 1] = distance[current[0]][current[1]] + 1
      que.put((abs_sign(key[0] - current[0]) + abs_sign(key[1] - (current[1] + 1)) + distance[current[0]][current[1] + 1], [(current[0], current[1] + 1)] + route))
    if current[1] != 0 and (int(table[current[0]][current[1] - 1]) == 2 or int(table[current[0]][current[1] - 1]) == 6):
      distance[current[0]][current[1] - 1] = distance[current[0]][current[1]] + 1
      que.put((abs_sign(key[0] - current[0]) + abs_sign(key[1] - (current[1] - 1)) + distance[current[0]][current[1] - 1],[(current[0], current[1] - 1)] + route))
    if current[0] != 0 and (int(table[current[0] - 1][current[1]]) == 2 or int(table[current[0] - 1][current[1]]) ==  6):
      distance[current[0] - 1][current[1]] = distance[current[0]][current[1]] + 1
      que.put((abs_sign(key[0] -  (current[0] - 1)) + abs_sign(key[1] - current[1]) + distance[current[0] - 1][current[1]], [(current[0] - 1, current[1])] + route))


    route = que.get()[1]
    current = route[0]
    table[current[0]][current[1]] = 1
    time += 1

  que = queue.PriorityQueue()
  table = [[map[j][i] for i in range(len(map[0]))] for j in range(len(map))]
  distance = [[0 for i in range(len(map[0]))] for j in range(len(map))]

  while current != end:
    if current[0] != (len(table) - 1) and (int(table[current[0] + 1][current[1]]) == 2 or int(table[current[0] + 1][current[1]]) == 4):
      distance[current[0] + 1][current[1]] = distance[current[0]][current[1]] + 1
      que.put(( abs_sign(end[0] - (current[0] + 1)) + abs_sign(end[1] - current[1]) + distance[current[0] + 1][current[1]], [(current[0] + 1, current[1])] + route))

    if current[1] != len(table[0]) - 1 and (int(table[current[0]][current[1] + 1]) == 2 or int(table[current[0]][current[1] + 1]) == 4):
      distance[current[0]][current[1] + 1] = distance[current[0]][current[1]] + 1
      que.put((abs_sign(end[0] - current[0]) + abs_sign(end[1] - (current[1] + 1)) + distance[current[0]][current[1] + 1], [(current[0], current[1] + 1)] + route))
    if current[1] != 0 and (int(table[current[0]][current[1] - 1]) == 2 or int(table[current[0]][current[1] - 1]) == 4):
      distance[current[0]][current[1] - 1] = distance[current[0]][current[1]] + 1
      que.put((abs_sign(end[0] - current[0]) + abs_sign(end[1] - (current[1] - 1)) + distance[current[0]][current[1]- 1], [(current[0], current[1] - 1)] + route))
    if current[0] != 0 and (int(table[current[0] - 1][current[1]]) == 2 or int(table[current[0] - 1][current[1]]) ==  4):
      distance[current[0] - 1][current[1]] = distance[current[0]][current[1]] + 1
      que.put((abs_sign(end[0] -  (current[0] - 1)) + abs_sign(end[1] - current[1]) + distance[current[0]-1][current[1]], [(current[0] - 1, current[1])] + route))


    route = que.get()[1]
    current = route[0]
    table[current[0]][current[1]] = 1
    time += 1

  return time, route



def first_floor():
  file = open("first_floor_input.txt", 'r')
  length = 0
  line = file.readline()
  info = line.split()
  table = [[] for i in range(0,int(info[1]))]

  for i in range(0,int(info[1])):
    table[i] = file.readline()
    table[i] = table[i].strip("\n")
    table[i] = table[i].split()

  file.close()

  start, end, key = find_index(info, table)

  #time, route = bfs(table, start, key, end)
  time, route = dfs(table, start, key, end)
  #time, route = greedy(table, start, key, end)
  #time, route = a_star(table, start, key, end)

  for index in route:
    length = length + 1
    if(int(table[index[0]][index[1]]) == 4):
      continue
    table[index[0]][index[1]] = 5

  file = open("first_floor_output.txt",'w')

  for i in range(int(info[1])):
    for j in range(int(info[2]) - 1):
      file.write(str(table[i][j]) + ' ')
    file.write(str(table[i][int(info[2]) - 1]) + '\n')
  file.write('---')
  file.write("\nlength=" + str(length))
  file.write("\ntime=" + str(time))
  file.close()

def second_floor():
  file = open("second_floor_input.txt", 'r')
  length = 0
  line = file.readline()
  info = line.split()
  table = [[] for i in range(0,int(info[1]))]

  for i in range(0,int(info[1])):
    table[i] = file.readline()
    table[i] = table[i].strip("\n")
    table[i] = table[i].split()

  file.close()

  start, end, key = find_index(info, table)

  #time, route = bfs(table, start, key, end)
  time, route = dfs(table, start, key, end)
  #time, route = greedy(table, start, key, end)
  #time, route = a_star(table, start, key, end)

  for index in route:
    length = length + 1
    if(int(table[index[0]][index[1]]) == 4):
      continue
    table[index[0]][index[1]] = 5

  file = open("second_floor_output.txt",'w')

  for i in range(int(info[1])):
    for j in range(int(info[2]) - 1):
      file.write(str(table[i][j]) + ' ')
    file.write(str(table[i][int(info[2]) - 1]) + '\n')
  file.write('---')
  file.write("\nlength=" + str(length))
  file.write("\ntime=" + str(time))
  file.close()

def third_floor():
  file = open("third_floor_input.txt", 'r')
  length = 0
  line = file.readline()
  info = line.split()
  table = [[] for i in range(0,int(info[1]))]

  for i in range(0,int(info[1])):
    table[i] = file.readline()
    table[i] = table[i].strip("\n")
    table[i] = table[i].split()

  file.close()

  start, end, key = find_index(info, table)

  #time, route = bfs(table, start, key, end)
  #time, route = dfs(table, start, key, end)
  time, route = greedy(table, start, key, end)
  #time, route = a_star(table, start, key, end)

  for index in route:
    length = length + 1
    if(int(table[index[0]][index[1]]) == 4):
      continue
    table[index[0]][index[1]] = 5

  file = open("third_floor_output.txt",'w')

  for i in range(int(info[1])):
    for j in range(int(info[2]) - 1):
      file.write(str(table[i][j]) + ' ')
    file.write(str(table[i][int(info[2]) - 1]) + '\n')
  file.write('---')
  file.write("\nlength=" + str(length))
  file.write("\ntime=" + str(time))
  file.close()

def fourth_floor():
  file = open("fourth_floor_input.txt", 'r')
  length = 0
  line = file.readline()
  info = line.split()
  table = [[] for i in range(0,int(info[1]))]

  for i in range(0,int(info[1])):
    table[i] = file.readline()
    table[i] = table[i].strip("\n")
    table[i] = table[i].split()

  file.close()

  start, end, key = find_index(info, table)

  #time, route = bfs(table, start, key, end)
  #time, route = dfs(table, start, key, end)
  time, route = greedy(table, start, key, end)
  #time, route = a_star(table, start, key, end)

  for index in route:
    length = length + 1
    if(int(table[index[0]][index[1]]) == 4):
      continue
    table[index[0]][index[1]] = 5

  file = open("fourth_floor_output.txt",'w')

  for i in range(int(info[1])):
    for j in range(int(info[2]) - 1):
      file.write(str(table[i][j]) + ' ')
    file.write(str(table[i][int(info[2]) - 1]) + '\n')
  file.write('---')
  file.write("\nlength=" + str(length))
  file.write("\ntime=" + str(time))
  file.close()

def fifth_floor():
  file = open("fifth_floor_input.txt", 'r')
  length = 0
  line = file.readline()
  info = line.split()
  table = [[] for i in range(0,int(info[1]))]

  for i in range(0,int(info[1])):
    table[i] = file.readline()
    table[i] = table[i].strip("\n")
    table[i] = table[i].split()

  file.close()

  start, end, key = find_index(info, table)

  #time, route = bfs(table, start, key, end)
  #time, route = dfs(table, start, key, end)
  time, route = greedy(table, start, key, end)
  #time, route = a_star(table, start, key, end)

  for index in route:
    length = length + 1
    if(int(table[index[0]][index[1]]) == 4):
      continue
    table[index[0]][index[1]] = 5

  file = open("fifth_floor_output.txt",'w')

  for i in range(int(info[1])):
    for j in range(int(info[2]) - 1):
      file.write(str(table[i][j]) + ' ')
    file.write(str(table[i][int(info[2]) - 1]) + '\n')
  file.write('---')
  file.write("\nlength=" + str(length))
  file.write("\ntime=" + str(time))
  file.close()
