# MeshCompare

A selection of tools to visualize differences between mesh objects.
These are visualized as vertex colors.

Currently there is only a python script that does a comparison on a single frame, but I will eventually add a deformer
node that will do the same dynamically.

This is still very rough, and performance is not my current goal but I'd like to get there.
So if you have any suggestions, please file an issue or, better yet, a pull request.

Specifically if someone has any ideas for better color schemes I'm all ears.

# Usage

## Python Script

1. Copy the `meshCompare.py` to your Maya scripts folder or anywhere in its `PYTHONPATH`
2. Now you can run the following python

    ```python
    from meshCompare import static_compare
    static_compare('pCube2', 'pCube1')
    ```
    
3. Adjust the `clamp` parameter to affect how strong the coloring is. 
   You can also adjust the `world` parameter to control whether comparisons happen in world space or object space.

## Deformer

The deformer will require to be compiled.
It defaults to Maya 2018 but you can specify a Maya Version by passing the `MAYA_VERSION` parameter to CMake.


To run it

```python
import maya.cmds as mc
mc.file(new=True, force=True)
try:
    mc.unloadPlugin('meshCompare', force=True)
finally:
    mc.loadPlugin("/Users/dhruv/Projects/meshCompare/build/meshCompare.bundle")
target = mc.polyCube()[0]
mesh = mc.polyCube()[0]

mc.polyOptions('pCubeShape2', colorShadedDisplay=True)
mc.setAttr('pCube1.visibility', 0)
deformer = mc.deformer('pCube2', type='meshCompare')[0]

mc.connectAttr('pCubeShape1.outMesh', '%s.target' % deformer)
mc.select('pCube2.vtx[4]')
mc.move(0, 10, 0.5, r=True)
mc.select(clear=True)

mc.viewFit()
```
