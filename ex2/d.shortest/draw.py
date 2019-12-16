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

x, y = getxy('time-1000.txt')
x2, y2 = getxy('time-2000.txt')
x3, y3 = getxy('time-2500.txt')
x4, y4 = getxy('time-3000.txt')
plt.xlabel('Process(n)')
plt.ylabel('Time(s)')
plt.plot(x, y)
plt.plot(x2, y2)
plt.plot(x3, y3)
plt.plot(x4, y4)
plt.legend(('points=1000', 'points=2000', 'points=2500', 'points=3000'))
plt.show()