package pizza;
import java.util.ArrayList;

public class PizzaShop {

	private volatile static PizzaShop singlePizzaShop;
	private int orderCounter;
	private ArrayList<Order> orders;
	private Oven pizzaOven;
	
	private PizzaShop(){
		orders = new ArrayList<Order>();
		pizzaOven = new Oven();
		orderCounter =1;
	}
	
	public static PizzaShop getInstance(){
		if(singlePizzaShop==null){
			synchronized(PizzaShop.class){
				if(singlePizzaShop==null){
					singlePizzaShop = new PizzaShop();
				}
			}
		}
		return singlePizzaShop;
	}
	
	public void placeOrder(PizzaOrTopping pizza){
		Order order = new Order(orderCounter,pizzaOven);
		orderCounter++;
		orders.add(order);
		pizzaOven.registerObserver(order);
		pizzaOven.addPizza(pizza);
	}
	
	public int getOrderCounter(){
		return orderCounter;
	}
	
	public Oven getPizzaOven(){
		return pizzaOven;
	}
	
	public String toString(){
		return pizzaOven.toString();
		
	}
}
