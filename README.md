# Programming II - HW2 | In Memory Student Register+

-------

## Usage:

    ./project2 <initial_array_size> <array_scale_factor> <initial_hash_table_size>

    
|Mandate|Entrance|output stdout|output stdout|Exit|
|-------|--------|-------------|-------------|----|
|-|-|(success)|(failure)|stderr|
|add|a `<aem>` `<name>` `<courses>`|A-OK `<aem>`, `<e> <c>`|A-NOK `<aem>`, `<e> <c>`|-|
|rmv|r `<aem>`|R-OK `<aem>`, `<e> <c>`|R-NOK `<aem>`, `<e> <c>`|-|
|mod|m `<aem>` `<courses>`|M-OK `<aem>`|M-NOK `<aem>`|-|
|sort|s|S-OK|-|`$<nof comps>`|
|find|f `<aem>`|F-OK `<name>` `<courses>`|F-NOK `<aem>`|`$<nof comps>`|
|print|p|-|-|-|
|clear|c|C-OK|-|-|
|reg|g `<aem>` `<course nr>`|G​-OK `<aem>` `<course nr>`|G​-NOK `<aem>` or G​-NOK `<course nr>`|-|
|unreg|u `<aem>` `<course nr>`|U-OK `<aem>` `<course nr>`|U-NOK `<aem>` or U-NOK `<course nr>`|-|
|isreg|i `<aem>` `<course nr>`|YES or NO|I-NOK `<aem>`|-|
|list-courses|l `<aem>`|*|L-ΝΟΚ `<aem>`|-|
|find-by-name|n `<name>`|**|N-NOK `<name>`|-|
|quit|q|-|-|-|

\* The list-courses command prints L-OK `<name>` and starting on the next line, one course record
`<course nr>` per line.

\*\* The find-by-name command prints the message N-OK `<name>` and starting from
next line, for each student with this name, his AEM is printed, character ' (space), the number of
courses he owes and character '\n'.

-------

## User Manual

**Compilation**: make compile

**Testing**: make run

**Clean**: make clean

-------------

