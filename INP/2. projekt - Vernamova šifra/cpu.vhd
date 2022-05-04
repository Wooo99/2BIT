-- cpu.vhd: Simple 8-bit CPU (BrainF*ck interpreter)
-- Copyright (C) 2020 Brno University of Technology,
--                    Faculty of Information Technology
-- Author(s): DOPLNIT
--

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity cpu is
 port (
   CLK   : in std_logic;  -- hodinovy signal
   RESET : in std_logic;  -- asynchronni reset procesoru
   EN    : in std_logic;  -- povoleni cinnosti procesoru
 
   -- synchronni pamet ROM
   CODE_ADDR : out std_logic_vector(11 downto 0); -- adresa do pameti
   CODE_DATA : in std_logic_vector(7 downto 0);   -- CODE_DATA <- rom[CODE\_ADDR] pokud CODE_EN='1'
   CODE_EN   : out std_logic;                     -- povoleni cinnosti
   
   -- synchronni pamet RAM
   DATA_ADDR  : out std_logic_vector(9 downto 0); -- adresa do pameti
   DATA_WDATA : out std_logic_vector(7 downto 0); -- ram[DATA_ADDR] <- DATA_WDATA pokud DATA_EN='1'
   DATA_RDATA : in std_logic_vector(7 downto 0);  -- DATA_RDATA <- ram[DATA_ADDR] pokud DATA_EN='1'
   DATA_WE    : out std_logic;                    -- cteni (0) / zapis (1)
   DATA_EN    : out std_logic;                    -- povoleni cinnosti 
   
   -- vstupni port
   IN_DATA   : in std_logic_vector(7 downto 0);   -- IN_DATA <- stav klavesnice pokud IN_VLD='1' a IN_REQ='1'
   IN_VLD    : in std_logic;                      -- data platna
   IN_REQ    : out std_logic;                     -- pozadavek na vstup data
   
   -- vystupni port
   OUT_DATA : out  std_logic_vector(7 downto 0);  -- zapisovana data
   OUT_BUSY : in std_logic;                       -- LCD je zaneprazdnen (1), nelze zapisovat
   OUT_WE   : out std_logic                       -- LCD <- OUT_DATA pokud OUT_WE='1' a OUT_BUSY='0'
 );
end cpu;

-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of cpu is

	------PC------
	signal pc : std_logic_vector (11 downto 0);
	signal inc : std_logic;
   signal dec : std_logic;
	signal id : std_logic;
  	------PTR------
	signal ptr : std_logic_vector (9 downto 0);
	signal p_inc : std_logic;
   signal p_dec : std_logic;
	-----MULTIPLEXOR-----
	signal mux_sel : std_logic_vector (1 downto 0) := (others => '0');
	signal mux_out : std_logic_vector (7 downto 0) := (others => '0');
	-----RAS-------
	signal ras : std_logic_vector (11 downto 0); --todo
	--todo CNT
	-----STATES-----
	type states is(
		start,
		inc_pointer,
		dec_pointer,
		inc_program,
		dec_program,
		lbracket,
		rbracket,
		write_s,
		get,
		fetch,
		decode,
		while_end,
		while_loop,
		while_condition,
		get_end,
		write_end,
		end_inc,
		end_dec,
		mux_inc_program,
		mux_dec_program,
		halt
	);
	signal c_states : states := start;	
	signal n_state : states ;
			
	

begin

	pc_process: process (CLK,RESET,inc,dec,id)
		begin
			if(RESET = '1')then
				pc <= (others => '0');
			elsif rising_edge(CLK) then
				if(inc = '1')then
					pc <= pc + 1;
				elsif(dec = '1')then
					pc <= pc -1;
				elsif(id = '1')then
					pc <=ras;
				end if;
			end if;
		end process;
	CODE_ADDR <= pc;
	
 	
	ptr_process: process (CLK,RESET,p_inc,p_dec)
		begin
			if(RESET = '1')then
				ptr <= (others => '0');
			elsif(rising_edge(CLK))then
				if(p_inc = '1')then
					ptr <= ptr + 1;
				elsif(p_dec = '1')then
					ptr <= ptr -1;
				end if;
			end if;
		end process;
	DATA_ADDR <= ptr;
	
	mux: process(CLK,RESET,mux_sel,mux_out)
		begin
			if(RESET = '1')then
				mux_out <= (others => '0');
			elsif(rising_edge(CLK))then
				case mux_sel is
					when"00" =>
						mux_out <= IN_DATA;
					when"01" =>
						mux_out <= DATA_RDATA + 1;
					when"10" =>
						mux_out <= DATA_RDATA - 1;
					when others =>
						mux_out <= (others => '0');
				end case;
			end if;
		end process;
		DATA_WDATA <= mux_out;
		OUT_DATA <= DATA_RDATA;

	logic : process (CLK,RESET,EN)
		begin
			if(RESET = '1')then
				c_states <= start;
			elsif(rising_edge(CLK))then
				if(EN = '1')then
					c_states <= n_state;
				end if;
			end if;
		end process;
	next_s_logic : process (c_states,IN_VLD,CODE_DATA,DATA_RDATA,OUT_BUSY)is
		begin
			mux_sel <= "00";
			inc <= '0';
			dec <= '0';
			id <= '0';
			p_inc <= '0';
			p_dec <= '0';
			CODE_EN <= '0';
			DATA_WE <= '0';
			DATA_EN <= '0';
			OUT_WE <= '0';
			IN_REQ <= '0';

		case c_states is
			when start =>
				n_state <= fetch;
			when fetch =>
				CODE_EN <= '1';
				n_state <= decode;
			when decode =>
				case CODE_DATA is
					when x"3E" =>
					n_state <= inc_pointer;
					when x"3C" =>
					n_state <= dec_pointer;
					when x"2B" =>
					n_state <= inc_program;
					when x"2D" =>
					n_state <= dec_program;
					when x"5B" =>
					n_state <= lbracket;
					when x"5D" =>
					n_state <= rbracket;
					when x"2E" =>
					n_state <= write_s;
					when x"2C" =>
					n_state <= get;
					when x"00" =>
					n_state <= halt;
					when others=>
					inc <='1';
					n_state <= decode;
				end case;
			when halt =>
				n_state <= halt;
			when inc_pointer =>
				inc <= '1';
				p_inc <= '1';
				n_state <= fetch;
			when dec_pointer =>
				inc <= '1';
				p_dec <= '1';
				n_state <= fetch;
			when inc_program =>
				DATA_EN <= '1';
				DATA_WE <= '0';
				n_state <= mux_inc_program;
			when mux_inc_program =>
				mux_sel <= "01";
				n_state <= end_inc;
			when end_inc =>
				inc <= '1';
				DATA_EN <= '1';
				DATA_WE <= '1';
				n_state <= fetch;
			when dec_program =>
				DATA_EN <= '1';
				DATA_WE <= '0';
				n_state <= mux_dec_program;
			when mux_dec_program =>
				mux_sel <= "10";
				n_state <= end_dec;
			when end_dec =>
				DATA_EN <= '1';
				DATA_WE <= '1';
				inc <= '1';
				n_state <= fetch;
			when write_s =>
				DATA_EN <= '1';
				DATA_WE <= '0';
				n_state <= write_end;
			when write_end =>
				if OUT_BUSY = '1' then
					DATA_EN <= '1';
					DATA_WE <= '0';
					n_state <= write_end;
				else
				OUT_WE <= '1';
				inc <= '1';
				n_state <= fetch;
				end if;
			when get =>
				IN_REQ <= '1';
				mux_sel <= "00";
				n_state <= get_end;
			when get_end =>
				if(IN_VLD = '1')then
					inc <= '1';
					DATA_EN <= '1';
					DATA_WE <= '1';
					n_state <= fetch;
				else
					IN_REQ <= '1';
					mux_sel <= "00";
					n_state <= get_end;
				end if;
			when rbracket =>
				if(DATA_RDATA /= "00000000")then
					id <= '1';
					n_state <= fetch;
				else
					inc <= '1';
					n_state <= fetch;
				end if;
			when lbracket =>
				inc <= '1';
				DATA_EN <= '1';
				ras <= pc;
				n_state <= while_condition;
			when while_condition =>
				if(DATA_RDATA /= "00000000")then
					ras <= pc;
					n_state <= fetch;
				else
					CODE_EN <= '1';
					n_state <= while_loop;
				end if;
			when while_loop =>
				inc <= '1';
				if(CODE_DATA = x"5D")then
					ras <= "000000000000";
					n_state <= fetch;
				else
					n_state <= while_end;
				end if;
			when while_end =>
				DATA_EN <= '1';
				n_state <= while_loop;
			when others =>						inc <= '1';
						n_state<= fetch;

				
		end case;
	end process;


end behavioral;
 
