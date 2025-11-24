# Dialogue File Format Guide

## Overview
Dialogues are stored in simple text files that are parsed at runtime to build interactive dialogue trees.

## Basic Syntax

### Comments
```
# This is a comment
# Comments are ignored by the parser
```

### Root Node Declaration
```
ROOT:node_id
```
Specifies which node is the starting point of the dialogue tree.

### Node Definition
```
NODE:unique_node_id
MSG:The message text that appears for this node
CHOICE:Choice text | parameters...
CHOICE:Another choice | parameters...
```

## Choice Parameters

Choices can have multiple parameters separated by `|`:

### Navigation
```
target:node_id
```
Specifies which node to navigate to when this choice is selected.

### Actions

#### Gold
```
gold:20          # Give 20 gold
gold:-30         # Take 30 gold (for purchases)
```

#### Items
```
item:ItemName:ItemType:bonus
```

Item types:
- `WEAPON` - Bonus adds to attack
- `ARMOR` - Bonus adds to defense
- `POTION` - Bonus restores health
- `CONSUMABLE` - Bonus restores health/mana
- `QUEST_ITEM` - Story item
- `MISC` - Other items

Examples:
```
item:Iron Sword:WEAPON:5          # +5 attack sword
item:Health Potion:POTION:50      # Restores 50 HP
item:Quest Map:QUEST_ITEM:0       # Quest item (no bonus)
```

#### Experience
```
xp:10           # Give 10 experience points
```

#### Health
```
health:50       # Restore 50 health
health:-20      # Deal 20 damage
```

#### Mana
```
mana:30         # Restore 30 mana
```

### Conditions
```
condition:expression
```

Conditions determine if a choice is available. If the condition evaluates to false, the action won't execute.

Supported conditions:
- `gold>=X` - Player has at least X gold
- `gold>X` - Player has more than X gold
- `mana>=X` - Player has at least X mana
- `level>=X` - Player is at least level X
- `hasitem:ItemName` - Player has the item in inventory

Multiple conditions can be chained on the same line. The choice will only be available if ALL conditions are met.

Examples:
```
CHOICE:Buy sword | condition:gold>=50 | gold:-50 | item:Sword:WEAPON:10
CHOICE:Advanced quest | condition:level>=5 | target:hard_quest
CHOICE:Use map | condition:hasitem:Quest Map | target:cave_entrance
```

## Complete Examples

### Simple Linear Dialogue
```
ROOT:greeting

NODE:greeting
MSG:Hello traveler! How can I help you?
CHOICE:Tell me about the town. | target:town_info
CHOICE:Goodbye. | target:farewell

NODE:town_info
MSG:This is a peaceful town with a market and inn.
CHOICE:Thanks for the info! | target:farewell

NODE:farewell
MSG:Safe travels!
```

### Shop with Conditional Purchases
```
ROOT:shop_greeting

NODE:shop_greeting
MSG:Welcome! I sell potions and weapons.
CHOICE:Show me potions | target:shop_potions
CHOICE:Show me weapons | target:shop_weapons
CHOICE:Goodbye | target:shop_goodbye

NODE:shop_potions
MSG:Health Potion: 15 gold, Mana Potion: 20 gold
CHOICE:Buy Health Potion | condition:gold>=15 | gold:-15 | item:Health Potion:POTION:50 | target:shop_greeting
CHOICE:Buy Mana Potion | condition:gold>=20 | gold:-20 | item:Mana Potion:POTION:30 | target:shop_greeting
CHOICE:Back to main menu | target:shop_greeting

NODE:shop_weapons
MSG:I have an Iron Sword for 50 gold and a Steel Sword for 100 gold.
CHOICE:Buy Iron Sword | condition:gold>=50 | gold:-50 | item:Iron Sword:WEAPON:5 | target:shop_greeting
CHOICE:Buy Steel Sword | condition:gold>=100 | gold:-100 | item:Steel Sword:WEAPON:10 | target:shop_greeting
CHOICE:Back to main menu | target:shop_greeting

NODE:shop_goodbye
MSG:Come again!
```

### Quest with Rewards
```
ROOT:quest_giver

NODE:quest_giver
MSG:I need someone brave to retrieve an artifact from the caves. Will you help?
CHOICE:Yes, I'll do it! | target:accept_quest | gold:50 | item:Quest Map:QUEST_ITEM:0 | xp:25
CHOICE:Tell me more | target:quest_details
CHOICE:Not interested | target:decline_quest

NODE:quest_details
MSG:The caves are dangerous, filled with goblins. But the reward is great!
CHOICE:I'll take the risk! | target:accept_quest | gold:50 | item:Quest Map:QUEST_ITEM:0 | xp:25
CHOICE:Too dangerous for me | target:decline_quest

NODE:accept_quest
MSG:Excellent! Here's a map and some gold for supplies. Good luck!

NODE:decline_quest
MSG:Perhaps another time. Safe travels.
```

### Advanced: Branching with Multiple Actions
```
ROOT:mysterious_stranger

NODE:mysterious_stranger
MSG:I can sense great potential in you, warrior. Let me offer you a gift.
CHOICE:Accept the gift | target:receive_gift | gold:100 | item:Magic Sword:WEAPON:15 | xp:50 | health:50
CHOICE:Refuse politely | target:refuse_gift

NODE:receive_gift
MSG:Take this enchanted blade and gold. Use them wisely. You also feel refreshed!

NODE:refuse_gift
MSG:A humble soul. Here, take this instead. | gold:20 | xp:10
```

## Best Practices

1. **Use descriptive node IDs**: `merchant_greeting` instead of `node1`
2. **Keep messages concise**: Long text may not fit on screen
3. **Test conditions**: Make sure players can actually meet the conditions
4. **Provide alternatives**: If a choice has a condition, offer an alternative
5. **Chain dialogues logically**: Ensure the flow makes narrative sense
6. **Use END nodes**: Have clear endpoints where conversations conclude

## Tips

- You can have the same target node from multiple choices
- Actions are executed in order before navigation
- If a condition fails, nothing happens (choice does nothing)
- Multiple items can be given in a single choice by chaining item actions
- Negative gold values represent purchases or costs
- XP is automatically converted to level ups (every 100 XP = 1 level)

## Error Handling

Common issues:
- **Node not found**: Check that target node IDs match NODE declarations
- **Item not created**: Verify item format (Name:Type:bonus)
- **Condition always fails**: Check player stats meet the condition
- **Missing ROOT**: First line should declare ROOT:node_id
