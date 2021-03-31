## Draw Dodecahedrons

A simple Open GL implementation for drawing 3 types of dodecahedrons: <br>
* Hexagonal Dipyramid
* Elongated square dipyramid
* Decagonal Prism

The code will prompt for the seclection of the dodecahedron to be displayed

The controls are as follows: <br>

* Moving the camera:

    * <button>A</button>  : -X direction
    * <button>D</button>  : +X direction
    * <button>W</button>  : +Y direction
    * <button>S</button>  : -Y direction
    * <button>Z</button>  : +Z direction
    * <button>X</button>  : -Z direction

* Moving the object:
    * <button>F</button>  : -X direction
    * <button>H</button>  : +X direction
    * <button>T</button>  : +Y direction
    * <button>G</button>  : -Y direction
    * <button>V</button>  : +Z direction
    * <button>B</button>  : -Z direction

* Spinning the object:
    * <button>N</button>  : Start Spinning
    * <button>M</button>  : Stop Spinning

* Spin the camera:
    * <button>O</button> : Start Spinning
    * <button>P</button> : Stop Spinning

* Teleportation:
    * <button>J</button>
    * <button>K</button>
    * <button>L</button>

* Reset:
    * <button>R</button>

### Note:

For the display of the texture of the Decagonal Prism, the `stb_imahe` header must be included (not a default C++ header, so either include it as a file, or add it to the headers and include it like a normal header, in this repo it is assumed to be added to the headers). Also, the texture image `wall.jpg` must be included in the folder in which the code is executing.
