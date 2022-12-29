-- generated file! do not edit.

while (true) do
  local YSTART = 0;
  local XSTART = 2;
  ypos = YSTART;
  xpos = XSTART;
  color = "#FFFFFF"
    local function wb(loc)
      local byte = memory.readbyte(loc);
      local hex = string.format("%2x", byte);
      gui.text(xpos, ypos, hex, color);
      xpos = xpos + 12;
      if xpos > 250 then
        xpos = XSTART;
        ypos = ypos + 8;
      end;
    end;


  -- score 0.000000
  xpos = XSTART;
  ypos = ypos + 10;
  color = "#ff4444"
  wb(14);
  wb(726);
  wb(842);
  wb(13);
  wb(49);
  wb(5);
  wb(945);
  wb(493);
  wb(18);
  wb(0);
  wb(773);
  wb(1279);
  wb(1);
  wb(774);
  wb(1013);
  wb(130);
  wb(25);
  wb(126);
  wb(69);
  wb(187);
  wb(125);
  wb(113);
  wb(650);
  wb(157);
  wb(64);

  -- score 0.000319
  xpos = XSTART;
  ypos = ypos + 10;
  color = "#44ff44"
  wb(1235);
  wb(772);
  wb(196);
  wb(193);
  wb(945);
  wb(773);
  wb(1002);
  wb(837);
  wb(774);
  wb(216);
  wb(920);
  wb(21);
  wb(217);
  wb(921);
  wb(80);
  wb(215);

  -- score 0.001173
  xpos = XSTART;
  ypos = ypos + 10;
  color = "#4444ff"
  wb(170);
  wb(702);
  wb(13);
  wb(1012);
  wb(53);
  wb(369);
  wb(970);
  wb(1013);
  wb(18);
  wb(0);
  wb(130);
  wb(88);
  wb(773);
  wb(172);
  wb(833);
  wb(157);
  wb(113);
  wb(187);
  wb(766);
  wb(1279);
  wb(126);
  wb(81);
  wb(58);
  wb(69);
  wb(145);
  wb(836);
  wb(774);
  wb(155);
  wb(25);
  wb(125);
  wb(64);

  -- score 0.005924
  xpos = XSTART;
  ypos = ypos + 10;
  color = "#ffff44"
  wb(978);
  wb(126);
  wb(837);
  wb(198);
  wb(945);
  wb(768);
  wb(1165);
  wb(1805);
  wb(781);
  wb(944);
  wb(242);
  wb(773);
  wb(1803);
  wb(1416);
  wb(1004);
  wb(181);
  wb(58);
  wb(774);
  wb(722);
  wb(157);
  wb(766);
  wb(213);
  wb(145);
  wb(193);
  wb(212);
  wb(210);
  wb(211);
  wb(21);
  wb(16);
  wb(618);
  wb(689);
  wb(525);
  wb(80);

  -- score 0.005956
  xpos = XSTART;
  ypos = ypos + 10;
  color = "#ff44ff"
  wb(59);
  wb(1820);
  wb(837);
  wb(945);
  wb(107);
  wb(125);
  wb(1821);
  wb(242);
  wb(112);
  wb(1004);
  wb(525);
  wb(1165);
  wb(248);
  wb(155);
  wb(1826);
  wb(781);
  wb(253);
  wb(1808);
  wb(1803);
  wb(782);
  wb(1827);
  wb(774);
  wb(157);
  wb(1700);
  wb(213);
  wb(546);
  wb(582);
  wb(148);
  wb(1807);
  wb(497);
  wb(252);

  -- score 0.015134
  xpos = XSTART;
  ypos = ypos + 10;
  color = "#44ffff"
  wb(169);
  wb(198);
  wb(69);
  wb(622);
  wb(34);
  wb(64);
  wb(193);
  wb(1805);
  wb(1165);
  wb(53);
  wb(249);
  wb(242);
  wb(1814);
  wb(525);
  wb(49);
  wb(782);
  wb(774);
  wb(709);
  wb(187);
  wb(157);
  wb(148);
  FCEU.frameadvance();
end;
