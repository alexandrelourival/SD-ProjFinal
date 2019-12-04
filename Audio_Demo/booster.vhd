------------------------------------------------------------------------------------------------
--  booster.vhd
------------------------------------------------------------------------------------------------
-- Etapa de amplificacion limpia.
-- Dado un valor parametrizado multiplica la senal de entrada y retorna dicha senal amplificada.
-- Se descartan los 9 bits mas significativos despues del signo para controlar la ganancia.
------------------------------------------------------------------------------------------------
-- Jose Angel Gumiel
--  25/11/2016
------------------------------------------------------------------------------------------------

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_vector.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_signed.all;

entity booster is
	port(
		--Datos:
		bleft_channel_audio_in: in std_logic_vector	(31 downto 0);
		bright_channel_audio_in: in std_logic_vector(31 downto 0);
		bCLOCK_50: in std_logic_1164; 
		
		bleft_channel_audio_out: out std_logic_vector(31 downto 0);
		bright_channel_audio_out: out std_logic_vector(31 downto 0);
	);
end entity booster;

architecture a of booster is

	component Audio_Controller
		port (
			CLOCK_50: in std_logic_1164;
			left_channel_audio_in: in std_logic_vector(31 downto 0);
			right_channel_audio_in: in std_logic_vector(31 downto 0);
			left_channel_audio_out: out std_logic_vector(31 downto 0);
			right_channel_audio_out: out std_logic_vector(31 downto 0);
		);
	end component;
	
	signal signal_unnormalized_right: std_logic_vector(31 downto 0);
	signal signal_unnormalized_left: std_logic_vector(31 downto 0);
	signal multiplier: std_logic_vector(31 downto 0):= "00000000100110001001011010000000";
	
	begin
		
		bleft_channel_audio_in: Audio_Controller PORT MAP(left_channel_audio_in);
		bright_channel_audio_in: Audio_Controller PORT MAP(right_channel_audio_in);
		bleft_channel_audio_out: Audio_Controller PORT MAP (left_channel_audio_out);
		bright_channel_audio_in: Audio_Controller PORT MAP (right_channel_audio_out);
		bCLOCK_50: Audio_Controller PORT MAP (CLOCK_50);
		
		signal_unnormalized_left<=signed(bleft_channel_audio_in)*signed(multiplier);
		signal_unnormalized_right<=signed(bright_channel_audio_in)*signed(multiplier);
		
		process(CLOCK_50,signal_unnormalized_left , signal_unnormalized_right)
		begin
			if (signal_unnormalized_left(31) = '1') then --resultado negativo
				bleft_channel_audio_out <= '1' & signal_unnormalized;
				bright_channel_audio_out <=  '1' & signal_unnormalized;
				
			else --resultado positivo
				bleft_channel_audio_out <= '0' & signal_unnormalized;
				bright_channel_audio_out <=  '0' & signal_unnormalized;
				
			end if;
			
			if (signal_unnormalized_right(31) = '1') then --resultado negativo
				bleft_channel_audio_out <= '1' & signal_unnormalized;
				bright_channel_audio_out <=  '1' & signal_unnormalized;
				
			else --resultado positivo
				bleft_channel_audio_out <= '0' & signal_unnormalized;
				bright_channel_audio_out <=  '0' & signal_unnormalized;
			
			end if;
	end process;
	
end architecture a;