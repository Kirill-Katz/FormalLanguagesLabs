A: VecF64
B: VecF64
C: VecF64

input: VecF64 = loadCSV("./some_csv.csv")

a = SumVec(A)
b = SumVec(B)
c = SumVec(C)

z = SumVals(3.3, 4.5)

temp = SumVals(a, b)
res = SumVals(temp, c)

return res
