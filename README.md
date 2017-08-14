# Renderers of Catan
Renderers of Catan is a [Catan](http://www.catan.com/) board generation tool. It can be used to generate a board image with various contraints and dimensions. The image can then be printed and played on!

## Configuration File
Different kinds of boards can be generated with config files. The format for the config file is given below. There are some sample config files in [configs](configs/)

```
{
  "board" : [<int>, ...],
  "chits" : [<int>, ...],
  "water_count" : <int>,
  "desert_count" : <int>,
  "gold_count" : <int>,
  "forest_count" : <int>,
  "mine_count" : <int>,
  "mountain_count" : <int>,
  "wheat_counmt" : <int>,
  "fields_count" : <int>,
  "rules" : [
    <rule object>,
    ...
  ],
  "ports" : [
    <port object>,
    ...
  ]
}
```

|Field|Required|Description|
|-----|--------|-----------|
|board|Yes|An array of integers, representing the dimensions of the board. Each number is the height of that column. Subsequent columns should have a difference that is odd. For example: (1, 2, 6, 9)|
|chis|Yes|An array of integers, representing the number of each chit. Value at index `i` in the chits array is the number of `i+1` chits. Size should be 12. For example, [0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0] means that there is only 1 chit of value 2, and 0 of others. index 0 and 6 should be left as 0.|
|water_count|Yes|Number of water hexes to use when randomly selecting hexes for the board.|
|desert_count|Yes|Number of desert hexes to use when randomly selecting hexes for the board.|
|gold_count|Yes|Number of gold hexes to use when randomly selecting hexes for the board.|
|forest_count|Yes|Number of forest hexes to use when randomly selecting hexes for the board.|
|mine_count|Yes|Number of mine hexes to use when randomly selecting hexes for the board.|
|mountain_count|Yes|Number of mountain hexes to use when randomly selecting hexes for the board.|
|wheat_count|Yes|Number of wheat hexes to use when randomly selecting hexes for the board.|
|fields_count|Yes|Number of field hexes to use when randomly selecting hexes for the board.|
|rules|Yes|Array of Rule objects (see below for available rules). Can be empty array.|
|ports|Yes|Array of Port objects (see below details). Can be empty array.|

Configuration files are specified with the `-c` command.

```
./catangen -c <path to config file>
```

## Rules
Rules are used to limit the kinds of board scenarios encountered when randomly generating boards. Below is a list of available rules, and what they do. The `follow_strictly` property specifies whether the rule should (if `false`). In the future we plan to use this to compute rankings for boards.

|Rule|Description|JSON format|
|----|-----------|-----------|
|NoPonds|Eliminates boards with 1 water tile completely surrounded by land.|`{ "name" : "NoPonds", "follow_strictly": <boolean> }`|
|NoSpecialChitsTouching|Eliminates boards that have two or more hexes with 8 or 6 next to eachother.|`{ "name" : "NoSpecialChitsTouching", "follow_strictly": <boolean> }`|
|NoSuperGold|Eliminates boards with 6 or 8 on gold.|`{ "name" : "NoSuperGold", "follow_strictly": <boolean> }`|
|IslandCount|Specifies the minimum or maximum (or both) of islands in the board, as well as the minimum size of each island. `min`, `max`, `minSize` are all optional.|`{ "name" : "NoPonds", "follow_strictly": <boolean>, "min" : <int>, "max" : <int>, "minSize" : <int> }`|

Increasing the number of rules for a config, or specific properties of the config can increase the time required to generate the board. It's also possible to specify a config file that is impossible to satisfy - for example a config file with only 1 land tile, and `IslandCount` specifying that there needs to be a minimum of two islands. Be careful! 

## Ports
Ports are specified by port objects as described below.

```
{
  "count" : <int>,
  "resource" : <string>
}
```

`count` refers to the number of that kind of port. `resource` is the kind of resource traded at that port. Below is a list of valid strings.

* "brick"
* "wheat"
* "sheep"
* "ore"
* "wood"
* "?"

## Installing
Before you start, you will need to install the following Homebrew packages.
* [glew](http://brewformulas.org/glew)
* [glfw](http://brewformulas.org/glfw)
* [libpng](http://brewformulas.org/libpng)
* [zlib](http://brewformulas.org/zlib)

Next, clone the repository. 
```
$> git clone https://github.com/0aix/RenderersOfCatan.git
```

Than, run `make` to build the program.
```
$> make
```

## Running
```
./catangen -c <path to config file>
```

The program will render in text and save as an image in the output directory. It will also open the image as soon as it's saved for previewing.

## Available Arguments
|Argument|Description|Required|Default|
|--------|-----------|--------|-------|
|`-c <path to config file>`|Specifies the path to the config file described above.|Yes|`n/a`|
|`-l <float value>`|Specifies the length of the hex tile sides in pixels|No|`400.0f`|
|`-r <float value>`|Specifies the radius of the chits in pixels|No|`100.0f`|
|`-m <float value>`|Specifies the length of the hex margins in pixels|No|`5.0f`|
|`-f <file name>`|Specifies the file name to save the board as. The board will be saved in the `output` folder|No|`board.png`|
|`-h`|Prints the available commands|No|`n/a`|

## Generated Board Examples
Here is an example of a board generated with the [seafarers config](configs/seafarers.json)

![](seafarers_board.jpg)

Here is an example of a board generated with the [cities and knights config](configs/citiesandknights.json)

![](candk_board.jpg)
