@echo off
for /l %%x in (3, 1, 5) do (
  for /l %%y in (3, 1, 8) do (
    enumerate_non_isomorphic %%x %%y > %%x_%%y_nk.txt
  )
)