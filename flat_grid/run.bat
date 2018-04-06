@echo off
for /l %%x in (6, 1, 7) do (
  for /l %%y in (7, 1, 10) do (
    generate_flat_grid %%x %%y > %%x_%%y.txt
  )
)