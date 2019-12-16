import matplotlib.pyplot as plt

def getxy(filename):
    x = []
    y = []
    with open(filename, 'r') as f:
        lines = f.readlines()
        for line in lines:
            line = line[0:-1]
            t1, t2 = line.split(' ')
            #print(t1, t2)
            x.append(eval(t1))
            y.append(eval(t2))
    return x,y

x, y = getxy('time.txt')
x2, y2 = getxy('../a-2/time.txt')
plt.xlabel('Process(n)')
plt.ylabel('Time(s)')
plt.plot(x, y)
plt.plot(x2, y2)
plt.legend(('buffer', 'tree'))
plt.show()