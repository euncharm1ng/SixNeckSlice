"""
hello world from me!
"""
import gui, go, goban, mctsEva

a = mctsEva.node("root")
b = mctsEva.node(a)
c = mctsEva.node(a)

print("start")

a.addC(b)
a.addC(c)
b.setName("im bc")
c.setName("im c")
a.printC()

print("end")


