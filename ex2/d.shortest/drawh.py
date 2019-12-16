import matplotlib.pyplot as plt

def getxy(filename):
    x = []
    y = []
    z = []
    with open(filename, 'r') as f:
        lines = f.readlines()
        for line in lines:
            line = line[0:-1]
            t1, t2, t3 = line.split(' ')
            #print(t1, t2)
            x.append(eval(t1))
            y.append(eval(t2))
            z.append(eval(t3))
    return x,y, z

x2, y2, z2 = getxy('time-3000h-2.txt')
x4, y4, z4 = getxy('time-3000h-4.txt')
x8, y8, z8 = getxy('time-3000h-8.txt')
plt.xlabel('Threads(n)')
plt.ylabel('Time(s)')
plt.plot(y2, z2)
plt.plot(y4, z4)
plt.plot(y8, z8)
plt.legend(('n=3000,processes=2', 'n=3000,processes=4', 'n=3000,processes=8'))
plt.show()