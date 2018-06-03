package pizza;
import java.util.ArrayList;
import java.util.Timer;
import java.util.TimerTask;

public class Oven implements Subject{
	
	private PizzaOrTopping finishedPizza;
	private ArrayList<PizzaOrTopping> pizzas;
	private ArrayList<Observer> observers;
	
	public Oven(){
		pizzas = new ArrayList<PizzaOrTopping>();
		observers = new ArrayList<Observer>();
	}
	
	public void registerObserver(Observer O){
		observers.add(O);
	}
	
	public void removeObserver(Observer O){
		observers.remove(O);
	}
	
	public void notifyObservers(){
		for(int i =0;i<observers.size();i++){
			observers.get(i).update(finishedPizza);
		}
	}
	
	public String toString(){
		String returns = "";
		for(PizzaOrTopping e : pizzas){
			returns += e.getDescription();
		}
		return returns;
	}

	public void removePizza(PizzaOrTopping pizza){
		pizzas.remove(pizza);
	}
	
	public void addPizza(PizzaOrTopping pizza){
		pizzas.add(pizza);
		
		Timer pizzaTimer = new Timer();
		pizzaTimer.schedule(new TimerTask(){
			public void run(){
				pizza.finish();
				finishedPizza = pizza;
				removePizza(pizza);
				notifyObservers();
			}
		},pizza.getCookingTime());
		
	}
}
