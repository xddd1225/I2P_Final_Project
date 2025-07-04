# Final Tank

> I2P(II) 2025 Final Project Team 38  
> 組員：陶威綸、胡佳希、謝承軒

## Description

*Final Tank* is a 2D tank battle game inspired by *Wii Play Tanks!*.  
The gameplay focuses on real-time strategy and shooting mechanics.
- An **AI agent** that can compete against a human player
- Player controls a tank using a keyboard (WASD) and mouse for aiming/shooting
- Press spacebar to dash for a short distance
- Press 'E' to place a landmine, which is able to cause damage to tanks(including player themself) and destroy soft walls
- **Bullets can bounce off the walls**, enabling player to come up with technical and trick shots
- Player can choose from default or **randomly generated map**
- Coins will be randomly generated across the map,  
  player can press 'O' to open store interface and spend coins to purchase utilities
  - $3 per utility
  - Utilities includes: increase moving speed, increase fire rate, and heal the tank
- The game ends when either the player's or the AI tank is destroyed
- A scoreboard to keep track of the history of each game

## Features

- [Easy]
  - 隨機生成金幣、商店購買道具
  - 遊戲暫停
  - Player移動邏輯（啟動、撞牆改變方向等）
  - 地雷引爆
  - 可破壞性地圖元素
  - 血量顯示
  - 坦克炮管跟隨滑鼠方向
  - 顯示玩家瞄準輔助線
- [Hard]
  - 隨機生成地圖
  - 蒙地卡羅演算法AI
  - 子彈反彈

## Individual Contribution

- **110021121 陶威綸** (40%)
  - 子彈、地雷邏輯、商店、金幣、AI坦克、玩家移動優化
- **112062171 胡佳希** (30%)
  - UI, Game assets (buttons, map tiles, tank models, background images, font, game icon)
  - In-game mechanisms: tank barrel movement, player mouse aiming line, tank explosion animation
  - Game over dialog
  - Audio (BGM, win/lose music, and SFX)
- **112062234 謝承軒** (30%)
  - map generation, tank motivation, collision detection, history recording, stage selection, health bar
