# Tilemap Editor 
This is a Tilemap editor that is to be used for making maps for the **Pikuma Game Engine**, by **Gustavo Pezzi**. 
https://pikuma.com/courses/cpp-2d-game-engine-development.

The Editor uses the **ECS** system from that course and you can load and save data to [.map] files.
When saving and loading, the editor uses a [.lua] file that loads the the location of the 
[.map] files and the tilesets that are used.

This application was created to quickly make tile maps for small games created with the **Pikuma Game Engine**.

## Technologies
*    **ImGui** - for user interface
*    **SDL2** - for rendering and window/inputs
*    **Sol/Lua** - for loading files
*    **ECS** - from Pikuma Game Engine 


## Functionality
* Load/Add multiple tilemaps
* Undo/Redo functionality when adding/removing tiles and changing canvas size
* Save/Load capabilities.
    *  I have changed the way that I load/save files. It differs from the course; therefore, you may have to rewrite the save functionality.
    *  The load function uses lua/sol and loads previously saved projects and their assets/tilemaps into the asset manager.
* Selecting different tiles based on where you click on the displayed tileset
* Panning and Zooming
 
## Tilemap Editor Controls
| key | Function | 
| --- | -------- |
| W   | Move Camara Up |
| S   | Move Camera Down |
| A   | Move Camera Left |
| D   | Move Camera Right |
| Space | Move Camera to Default Position |
| Ctrl + Z | Undo Command |
| Ctrl + Shift + Z | Redo Command | 
| Ctrl + S | Save/Save As Project |
| Ctrl + O | Open Project |
| Ctrl + N | New Canvas/Project | 

| Mouse Button | Function |
| ------------ | -------- | 
| Left Button | Add Tile |
| Middle Button | Pan/Move the Camera |
| Right Button | Remove Tile |
| Wheel Up | Zoom In | 
| Wheel down | Zoom Out

