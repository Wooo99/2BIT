library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use IEEE.std_logic_unsigned.all;

-- rozhrani Vigenerovy sifry
entity vigenere is
   port(
         CLK : in std_logic;
         RST : in std_logic;
         DATA : in std_logic_vector(7 downto 0);
         KEY : in std_logic_vector(7 downto 0);

         CODE : out std_logic_vector(7 downto 0)
    );
end vigenere;

architecture behavioral of vigenere is
	
	signal Shift: std_logic_vector(7 downto 0);
	signal AfterPlus: std_logic_vector(7 downto 0);
	signal AfterMinus: std_logic_vector(7 downto 0);

	type types is (add, off);
	signal Now : types := add;
	signal NextTo : types := off;

	signal Fsm: std_logic_vector(1 downto 0);

	signal hash: std_logic_vector(7 downto 0) := "00100011";

begin

	valueProcess: process (DATA,KEY) is
		begin 
				shift <=KEY-64;
		end process;
	
	plus: process (Shift,DATA) is
		variable position: std_logic_vector(7 downto 0);
		begin
			position := DATA + Shift;
			if (position > 90)then
				position:= position -26;
			end if;
			AfterPlus <= position;
		end process;
	
	minus: process (Shift,DATA) is
		variable position: std_logic_vector(7 downto 0);
		begin
			position:= DATA - Shift;
			if (position < 65)then
					position:= position +26;
			end if;
			AfterMinus <= position;
		end process;
	
	Logic: process (CLK,RST) is
		begin
			if(RST = '1')then
				Now <= add;
			elsif rising_edge(CLK)then
				Now <= NextTo;
			end if;
		end process;

	Mealy: process (Now, DATA, RST) is
		begin
			case Now is 
				when add =>
					NextTo <= off;
					Fsm <="10";
				when off =>
					NextTo <= add;
					Fsm <="01";
			end case;

			if(RST = '1' )then
				Fsm <= "11";
			end if;

			if(DATA > 47 and DATA < 58) then
				Fsm <= "11";
			end if;
		end process;
	Multiplexor: process (Fsm,AfterMinus, AfterPLus) is
		begin
			case Fsm is
				when "01" =>
					CODE<=AfterMinus;
				when "10" =>
					CODE<=AfterPlus;
				when others =>
					CODE<=hash;
			end case;
		end process;


end behavioral;

