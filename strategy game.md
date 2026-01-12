# OUTDATED

3 levels:
- Unit
256x256m 1m grid, one level, place object, pathing via all free space is node then optimized nodes
production is medianized over time via work spot vs work schedule vs pathing.
Production is production at optimal supply and worker amount.
- Building:
1024x1024x1024m 2m grid, place units and paths. Pathing via path intersection nodes.
production is addition of units and medianized over time vs pathing.
Scale unit production vs supply per output vs input (lowest input define highest output of dependent output), and number of worker vs demand (prioritary over supply).
Buildings can have priority.

---- up until this point, simulation is done out of main game, ex ~ 1 week of production is simulated at optimal supply and then scaled in the world

- World:
int32 x int32, 4m grid, place templates of many buildings or buildings. pathing via path intersection nodes.
production is addition of buildings vs supply (outputs of other buildings) accessible via paths.

Scaling:
input table, output table
data table indicating what depends on what and in which ratio (eg, 2 iron ore and 1 coal ore makes 1 steel)
when not enough of a component, scale by ratio of that component (eg, we have 1 iron ore input, consume 1 iron ore, 0.5 coal ore, produce 0.5 steel).
Workers are prioritary and scale the whole building (only 50 % of workers, scale maximum production to 50 %)

Types of paths inside buildings:
- Walkway
- Forklift path
- Internal railway
- Lifts
etc

Types of paths outside buildings:
- Road
Gravel road
Asphalt road
Double asphalt road
etc

- Railroad
Gravel railroad
Solid railroad
Monorail
Maglev
etc

Special buildings:
- Airport
Can demand packet for other airports for unfulfilled local packet demands, remote airport then triggers a packet to itself, then the normal pathing applies between the 2 airports.
Contains airplanes for the job
- Spaceport
same but for space

Each have capacity, sends packet of x size of material at defined speed to buildings or units with paths connected vs available vehicles in depot (or workers in transporter unit for walkway)
Simplified as output to input, each transport unit has a progression that indicates where it is on its path, could be used to illustrate.
including workers for buildings, which are a supply that resets each cycle to the population number
