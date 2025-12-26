# Scene Files

This directory contains scene data files for the Runa2 game engine.

## Format

Scene files use a simple text format with space-separated tile indices.

### Example

```
# Comments start with #
# Each number represents a tile index from the tileset
# Spaces separate tiles horizontally
# Newlines separate rows vertically

0 1 2 3 4 5
6 7 8 9 10 11
12 13 14 15 16 17
```

## Creating Scenes

1. **Know your tileset** - Check the manifest YAML file for tile indices
   - Example: `plains_tile_0` = tile 0, `plains_tile_1` = tile 1, etc.

2. **Plan your dimensions** - Decide on map size
   - `sample_scene.txt` is 40x30 tiles (640x480 pixels at 16px/tile)

3. **Layout tiles** - Use tile indices to build your scene
   ```
   0 0 0 0 0  # Row of grass tiles
   36 36 36 36 36  # Row of path tiles
   ```

4. **Load in code**
   ```cpp
   std::ifstream file("Resources/scenes/my_scene.txt");
   std::stringstream buffer;
   buffer << file.rdbuf();
   tileMap->loadFromString(buffer.str());
   ```

## Plains Tileset Reference

The plains tileset (96x192, 16x16 tiles) contains 72 tiles:

- **0-5**: Basic grass variations
- **6-11**: Grass with flowers/details
- **12-17**: Grass borders (top)
- **18-23**: Grass borders (sides)
- **24-29**: Grass borders (bottom)
- **30-35**: Grass corners
- **36-47**: Path/dirt tiles
- **48-59**: Water edges
- **60-71**: Decorative elements

### Common Patterns

**Basic grass area:**
```
0 1 2 0 1 2
1 0 1 2 0 1
2 1 0 1 2 0
```

**Path through grass:**
```
0 1 2 0 1 2
36 36 36 36 36 36
0 1 2 0 1 2
```

**Grass with flowers:**
```
6 7 8 9 6 7
7 8 9 6 7 8
8 9 6 7 8 9
```

## Tools

You can create scene files with:
- **Text editor** - Manual tile placement
- **Tiled Map Editor** - Export as CSV, then convert to space-separated format
- **Custom tools** - Generate procedurally or from heightmaps

## Tips

1. **Use comments** - Document your tile placement strategy
2. **Consistent spacing** - Align columns for readability
3. **Test incrementally** - Build small sections first
4. **Layer approach** - Create base terrain, then add details
5. **Reuse patterns** - Copy-paste common tile arrangements

## Example Scenes

- [sample_scene.txt](sample_scene.txt) - 40x30 demo scene with grass, paths, and decorative elements
