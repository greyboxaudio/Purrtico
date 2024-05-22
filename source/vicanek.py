import math

Fs = 48000
F0 = 8000
w0 = 2*math.pi*F0/Fs
print(w0)

Q = 0.7
q = 1/(2*Q)
print(q)

db = 6
G = 10**(db/20)
print(G)

#impulse invariance (eq 12)
if q <= 1.0:
    a1 = -2*(math.e**(-1*q*w0))*math.cos(math.sqrt(1-(q**2))*w0)
else:
    a1 = -2*(math.e**(-1*q*w0))*math.cosh(math.sqrt((q**2)-1)*w0)
a2 = math.e**(-2*q*w0)

#(eq 26)
p0 = 1 - math.sin(w0/2)**2
p1 = math.pow(math.sin(w0/2),2)
p2 = 4*p0*p1

#(eq 27)
A0 = (1+a1+a2)**2
A1 = (1-a1+a2)**2
A2 = -4*a2

#(eq 44)
R1 = (A0*p0+A1*p1+A2*p2)*G**2
R2 = (-1*A0+A1+4*(p0-p1)*A2)*G**2

#(eq 45)
B0 = A0
B2 = (R1-R2*p1-B0)/(4*p1**2)
B1 = R2+B0+4*(p1-p0)*B2

#(eq 29)
W = 0.5*(math.sqrt(B0)+math.sqrt(B1))
b0 = 0.5*(W+math.sqrt(W**2+B2))
b1 = 0.5*(math.sqrt(B0)-math.sqrt(B1))
b2 = -1*B2/(4*b0)

print("b0 =",b0)
print("b1 =",b1)
print("b2 =",b2)
print("a1 =",a1)
print("a2 =",a2)