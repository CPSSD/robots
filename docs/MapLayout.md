# Layouts:
#### 1. Bitmap
* 2D Array of the map using points<br />
```
[[1, 1, 1, 1, 0],
 [0, 0, 0, 1, 1],
 [0, 0, 0, 0, 1],
 [0, 0, 0, 0, 1],
 [1, 1, 1, 1, 1]]
```

* The bigger the room the bigger the array.

#### 2. Vector
* 1D Array of map using lines<br />
```
[(0, 0, 3, 0), (3, 0, 3, 1), (3, 1, 4, 1), (4, 1, 4, 4), (4, 4, 0, 4)]
```

* Size of room isn't correlated to amount of lines. More complex geometry means more lines.

***

## Layout Info:
* The Bitmap would be used locally on the edison
* It would then be converted to a Vector for use on the Uno to conserve memory

## Possible Limitations:
*  No Circles.
*  Horizontal/Vertical Lines only.

## Questions:
* How much area should each point represent? 1cm³, 2cm³, etc..?
