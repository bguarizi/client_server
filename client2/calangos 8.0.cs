//--------------------------------> Leitura dos Sensores <------------------------------//

public static class LerSensores {

   public static Tuple<string, string, string, string, string, string, string> LerCor() {
		string rc3 = (Bot.GetComponent<ColorSensor>("rc3").Analog).ToString();
		string rc2 = (Bot.GetComponent<ColorSensor>("rc2").Analog).ToString();
		string rc1 = (Bot.GetComponent<ColorSensor>("rc1").Analog).ToString();
		string mc1 = (Bot.GetComponent<ColorSensor>("mc1").Analog).ToString();
		string lc1 = (Bot.GetComponent<ColorSensor>("lc1").Analog).ToString();
		string lc2 = (Bot.GetComponent<ColorSensor>("lc2").Analog).ToString();
		string lc3 = (Bot.GetComponent<ColorSensor>("lc3").Analog).ToString();

      	return Tuple.Create(rc2, rc1, mc1, lc1, lc2, rc3, lc3);
   }

	public static Tuple<double, double, double, double, double, double, double> LerLuz() {
		double rc3 = (Bot.GetComponent<ColorSensor>("rc3").Analog ).Brightness;
		double rc2 = (Bot.GetComponent<ColorSensor>("rc2").Analog ).Brightness;
		double rc1 = (Bot.GetComponent<ColorSensor>("rc1").Analog ).Brightness;
		double mc1 = (Bot.GetComponent<ColorSensor>("mc1").Analog ).Brightness;
		double lc1 = (Bot.GetComponent<ColorSensor>("lc1").Analog ).Brightness;
		double lc2 = (Bot.GetComponent<ColorSensor>("lc2").Analog ).Brightness;
		double lc3 = (Bot.GetComponent<ColorSensor>("lc3").Analog ).Brightness;

      return Tuple.Create(rc2, rc1, mc1, lc1, lc2, rc3, lc3);
   }

	public static Tuple<double, double, double, double> LerAzul() {
		double rc2 = ( (Bot.GetComponent<ColorSensor>("rc2").Analog).Blue );
		double rc1 = ( (Bot.GetComponent<ColorSensor>("rc1").Analog).Blue );
		double lc1 = ( (Bot.GetComponent<ColorSensor>("lc1").Analog).Blue );
		double lc2 = ( (Bot.GetComponent<ColorSensor>("lc2").Analog).Blue );

      return Tuple.Create(rc2, rc1, lc1, lc2);
   }

	public static Tuple<double, double, double, double> LerVerde() {
		double rc2 = ( (Bot.GetComponent<ColorSensor>("rc2").Analog).Green );
		double rc1 = ( (Bot.GetComponent<ColorSensor>("rc1").Analog).Green );
		double lc1 = ( (Bot.GetComponent<ColorSensor>("lc1").Analog).Green );
		double lc2 = ( (Bot.GetComponent<ColorSensor>("lc2").Analog).Green );

      return Tuple.Create(rc2, rc1, lc1, lc2);
   }

	public static Tuple<double, double, double, double> LerVermelho() {
		double rc2 = ( (Bot.GetComponent<ColorSensor>("rc2").Analog).Red );
		double rc1 = ( (Bot.GetComponent<ColorSensor>("rc1").Analog).Red );
		double lc1 = ( (Bot.GetComponent<ColorSensor>("lc1").Analog).Red );
		double lc2 = ( (Bot.GetComponent<ColorSensor>("lc2").Analog).Red );

      return Tuple.Create(rc2, rc1, lc1, lc2);
   }
}

//--------------------------------> Definição do Motor <------------------------------//

public static class Motor {

	public static async void Travar (bool esquerda, bool direita) { await Time.Delay(50);
		Bot.GetComponent<Servomotor>("lmotor1").Locked = esquerda; 
		Bot.GetComponent<Servomotor>("rmotor1").Locked = direita;
		Bot.GetComponent<Servomotor>("lmotor2").Locked = esquerda; 
		Bot.GetComponent<Servomotor>("rmotor2").Locked = direita;
	}

	public static async void Frente (int forca, int velocidade) { await Time.Delay(50);
	 	Bot.GetComponent<Servomotor>("lmotor1").Apply(forca, velocidade);
		Bot.GetComponent<Servomotor>("rmotor1").Apply(forca, velocidade);
	}

	public static async void Tras (int forca, int velocidade) { await Time.Delay(50);
	 	Bot.GetComponent<Servomotor>("lmotor1").Apply(forca, -(velocidade));
		Bot.GetComponent<Servomotor>("rmotor1").Apply(forca, -(velocidade));
	}

	public static async void Virar (int forca, int vel_esquerda, int vel_direita) { await Time.Delay(50);
		Bot.GetComponent<Servomotor>("lmotor1").Apply(forca, vel_esquerda);
		Bot.GetComponent<Servomotor>("rmotor1").Apply(forca, vel_direita);
	}

	public static async void LevantarGarra (int forca, int velocidade) { await Time.Delay(50);
		Bot.GetComponent<Servomotor>("gmotor").Locked = false;

		Bot.GetComponent<Servomotor>("gmotor").Apply(forca, velocidade);
		await Time.Delay(1000);

		Bot.GetComponent<Servomotor>("gmotor").Locked = true; 
	}

	public static async void SubirRampa (int forca, int velocidade) { await Time.Delay(50);
	 	Bot.GetComponent<Servomotor>("lmotor1").Apply(forca, velocidade);
		Bot.GetComponent<Servomotor>("rmotor1").Apply(forca, velocidade);
	 	Bot.GetComponent<Servomotor>("lmotor2").Apply(forca, velocidade);
		Bot.GetComponent<Servomotor>("rmotor2").Apply(forca, velocidade);
	}

	public static async void EixoDireita (int forca, int velocidade) { await Time.Delay(50);
		Bot.GetComponent<Servomotor>("rmotor1").Locked = true;
		Bot.GetComponent<Servomotor>("lmotor1").Apply(forca, velocidade);
	}

	public static async void EixoEsquerda (int forca, int velocidade) { await Time.Delay(50);
		Bot.GetComponent<Servomotor>("lmotor1").Locked = true;
		Bot.GetComponent<Servomotor>("rmotor1").Apply(forca, velocidade);
	}
}

//--------------------------------> Funções <------------------------------//

public static int FiltroVerde () {

	if ((LerSensores.LerAzul().Item2 <= 65.0 && LerSensores.LerVermelho().Item2 <= 60.0 && LerSensores.LerVerde().Item2 >= 70.0) && (LerSensores.LerAzul().Item3 <= 65.0 && LerSensores.LerVermelho().Item3 <= 60.0 && LerSensores.LerVerde().Item3 >= 70.0)) {
		return 3;
	}
	else if ((LerSensores.LerAzul().Item2 <= 65.0 && LerSensores.LerVermelho().Item2 <= 60.0 && LerSensores.LerVerde().Item2 >= 70.0) || (LerSensores.LerCor().Item2 == "Verde")) {
		return 2;
	}
	else if ((LerSensores.LerAzul().Item3 <= 65.0 && LerSensores.LerVermelho().Item3 <= 60.0 && LerSensores.LerVerde().Item3 >= 70.0) || (LerSensores.LerCor().Item3 == "Verde")) {
		return 1;
	}
	else if ((LerSensores.LerAzul().Item3 <= 30.0 && LerSensores.LerVermelho().Item3 <= 30.0 && LerSensores.LerVerde().Item3 >= 40.0) || (LerSensores.LerCor().Item3 == "Verde")) {
		return 4;
	}
	else if ((LerSensores.LerAzul().Item2 <= 30.0 && LerSensores.LerVermelho().Item2 <= 30.0 && LerSensores.LerVerde().Item2 >= 40.0) || (LerSensores.LerCor().Item2 == "Verde")) {
		return 5;
	}
	else {
		return 0;
	}
}

//--------------------------------> Setup <------------------------------//
async Task Main() {

	Motor.Travar(false,false);

	Motor.LevantarGarra(100,80);

	while(true) {

		await Time.Delay(50);
		
		var Cor = LerSensores.LerCor();
		var Luz = LerSensores.LerLuz();
		var LuzAzul = LerSensores.LerAzul();
		var LuzVerde = LerSensores.LerVerde();
		var LuzVermelho = LerSensores.LerVermelho();

		IO.Print(Cor.Item1 + " / " + Cor.Item2 + " / " + Cor.Item3 + " / " + Cor.Item4 + " / " + Cor.Item5 + " / " + Cor.Item6 + " / " + Cor.Item7);

		switch ( FiltroVerde() ) {
//--------------------------------------------------------------------------//
		case 0:
			// seguidor de linha

		if (Cor.Item1 == "Preto" && Cor.Item2 == "Preto") { // 90° à direita
			IO.Print("90° à direita");

			if (Cor.Item6 != "Branco") {
				IO.Print("É mentira!");

				Motor.Frente(100,75);
				await Time.Delay(1500);
			}
			else {
				while (Cor.Item6 != "Preto") { await Time.Delay(50);
					Motor.Virar(100,150,-150);

					Cor = LerSensores.LerCor();
				}
			}
		}
		else if (Cor.Item4 == "Preto" && Cor.Item5 == "Preto") { // 90° à esquerda
			IO.Print("90° à esquerda");

			if (Cor.Item7 != "Branco") {
				IO.Print("É mentira!");

				Motor.Frente(100,75);
				await Time.Delay(1500);
			}
			else {
				while (Cor.Item7 != "Preto") { await Time.Delay(50);
					Motor.Virar(100,-150,150);

					Cor = LerSensores.LerCor();
				}
			}
		}
		
		else if (Cor.Item1 == "Preto" && Cor.Item5 == "Preto") { // encruzilhada

			while (Cor.Item2 == "Preto" && Cor.Item4 == "Preto") { await Time.Delay(50);
				Motor.Frente(100,150);

				Cor = LerSensores.LerCor();
			}
		}

		else if (Cor.Item2 == "Branco" && Cor.Item4 == "Branco") {
			Motor.Frente(100,75);
		}

		else if (Cor.Item2 == "Branco" && Cor.Item4 == "Preto") {
			Motor.Virar(100,-150,150);
		}

		else if (Cor.Item2 == "Preto" && Cor.Item4 == "Branco") {
			Motor.Virar(100,150,-150);
		}
		break;

//--------------------------------------------------------------------------//
		case 1:
			// verde à esquerda

			IO.Print("Verde à Esquerda!");

			while (Cor.Item7 != "Preto") { await Time.Delay(50);
				Motor.Virar(100,-150,150);

				IO.Print("Virando...");
				Cor = LerSensores.LerCor();
			}
			break;

//--------------------------------------------------------------------------//
		case 2:
			// verde à direita

			IO.Print("Verde à Direita!");

			while (Cor.Item6 != "Preto") { await Time.Delay(50);
				Motor.Virar(100,-150,150);

				IO.Print("Virando..."); 
				Cor = LerSensores.LerCor();
			}
			break;

//--------------------------------------------------------------------------//
		case 3:
			// beco sem saída :(

			IO.Print("Beco sem saída, ah não kkk");

			while (Bot.Compass >= 350.0) { await Time.Delay(50); // para acertar a angulação em 0°
				Motor.Virar(100,150,-150);

				IO.Print("Bússola: " + Bot.Compass);
			}

			while (Bot.Compass <= 70.0) { await Time.Delay(50);
				Motor.Virar(100,277,-150);

				IO.Print("Bússola: " + Bot.Compass);
			}

			while (Bot.Compass <= 172.5) { await Time.Delay(50);
				Motor.Virar(100,150,-300);

				IO.Print("Bússola: " + Bot.Compass);
			}

			Motor.Frente(100,75);
			await Time.Delay(1000); // para não entrar no beco de novo
		break;

		}
	}
}