@echo off
for /l %%x in (5, 1, 6) do (
  for /l %%y in (2, 1, 8) do (
    noma_dimention %%x %%y > %%x_%%y.txt
  )
)