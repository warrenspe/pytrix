import sys
sys.path.append('build/lib.cygwin-2.4.1-x86_64-2.7')
import pytrix
v=pytrix.Vector([1, 2, 3])
v2=pytrix.Vector([1, 2, 3])
print v.dot(v2)

print v.cross(v2)
