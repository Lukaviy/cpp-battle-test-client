# Требования от геймдизов
На карте есть юниты которые могут перемещаться и атаковать кого нибудь.
Нужно дать возможность геймдизайнерам максимально разнообразить геймплей.

Возможны разные варианты перемещения, должно быть кастомизируемо.
На одной клетке может находится больше одного юнита, нужно дать настроить маску коллизий типов юнитов.

Атаки могут быть сильно разными, помимо прямого нанесения урона они могут спавнить других юнитов, накладывать эффекты и т. д.

Эффекты тоже могут быть сильно разными, также помимо простого нанесения урона, баффов или дебаффов они могут спавнить юнитов, менять коллизию, позицию и т. д.

## Симуляция
Каждый юнит ходит по отдельности в порядке создания.
В требованиях указано - что одним ходом считается одно действие одного юнита.
В то же время в исходной реализации есть понятие шага, за который ходят все юниты в порядке создания.
Нужно добавить юнит - охотник, который отравляет противника.
Указано что урон от яда должен наноситься каждый ход. И тут нужно понять, урон будет наносится каждый ход юнита который отравлен, или каждый ход любого юнита, или в начале каждого шага симуляции.
Урон будет в начале или в конце хода.

## Комбинирование эффектов и порядок действий

Порядок действий:
По опыту игры в пошаговые стратегии обычно все эффекты срабатывают до действия игрока.
Например, если юнит отравлен и у него остался 1 HP то он умрет не успев выполнить действие.
Но представим что пути геймдизайнеров неисповедимы. Обычно так и есть.

Разберем примеры которые могут прийти в голову.
- Огонь и вода.
	Оба эффекта убирают друг друга. Если юнит в огне и его обливают водой, то он перестает гореть. Если юнит в воде и его поджигают, то он перестает быть мокрым.

- Ближняя атака и огонь.
	Если юнит атакует в ближнем бою и при этом находится в огне, то он наносит дополнительный урон от огня.

- Ближняя атака и отравление.
	Если юнит атакует в ближнем бою и при этом отравлен, то с некоторой вероятностью он передаст эффект от отравления.
	Если на атакуемом есть эффект кровотечения, то шанс передать отравление удваивается.

- Огонь и кровотечение.
	Если юнит в огне то с него снимается кровотечение и он не может его получить.

- Броня и кровотечение.
	Если юнит в броне, он не может получить кровотечение. Если кровотечение уже было то оно остается.

- Гипергидроз.
	Накладывает эффект воды на юнита. Если юнит уже в воде, спавнит под юнитом лужу. Лужа исчезает спустя 3 хода.

- Электричество и вода.
	Если юнит в воде и получает электрический удар, то он получает дополнительный урон.

- Топологический кретинизм.
	Вместо выполнения хода в направлении цели с некоторым шансом может сделать ход в рандомном направлении.

- Хаотичная телепортация.
	Перед выполнением хода юнит может телепортироваться в случайную клетку которую может занять. Длится какое то количество ходов.

- TankBuster.
	Наносит процентный урон от текущего уровня HP.

- Отравление огонь и кровоточение.
	Если на юните есть все эти эффекты одновременно то в начале своего хода он взрывается. Наносит урон по области и умирает.

Эффекты в теории могут накладывать другие эффекты.
Результат хода зависит от порядка выполнения эффектов.

Если игрока атаковали фаерболом и наложили эффект горения.
Когда сработает эффект горения, до срабатывания эффекта гипергидроза или после?
Если до, то он успеет получить урон от огня. Если после, то огонь потушится эффектом воды и он не получит урон.
Результат должен быть всегда детерминированным, и нужна возможность настраивать порядок выполнения эффектов.

Что по поводу процентного урона. Он должен высчитываться от текущего уровня хп после применения нескольких эффектов, или от уровня хп в начале хода?

Можно сделать несколько фаз применения эффектов и задать их четкую последовательность.
Раунд - каждый юнит выполняет один ход в порядке создания.
Ход - действие юнита.

1. Начало раунда.
2. Начало хода юнита.
3. Ход юнита.
4. Конец хода юнита.
5. Конец раунда.

Для эффектов будет несколько событий:
1. Момент наложения.
	- В этот момент происходит взаимодействие эффектов.
	Например, если юнит в огне и его обливают водой, то он перестает гореть. Если юнит в воде и его поджигают, то он перестает быть мокрым и например накладывает еще эффект пара.
2. Применение эффектов.
	- В этот момент происходит применение эффектов. Например, если юнит отравлен то он получает урон.
3. Момент снятия.
	- Тут не придумал пока, но пусть будет.
4. Момент нанесения урона.
5. Момент получения урона.

Модификация атаки:
Есть состояние проводимой атаки.
1. Юнит источник урона.
2. Юнит цель.
3. Базовый урон.
4. Модификаторы урона.

Её могут модифицировать как эффекты находящиеся на атакующем юните, так и на цели.
Например, если юнит в огне то он наносит дополнительный урон от огня. Если юнит в воде и получает электрический удар, то он получает дополнительный урон.

# Варианты архитектуры для проекта:
## Entity Component
Базовый варик как в юнити, есть объект Entity который хранит в себе компоненты.
Компоненты хранят данные и логику.
Компоненты могут запрашивать какие еще компоненты есть на Entity и что нибудь с ними делать.
### Плюсы:
Базовая реализация максимально простая, те кто работал с Unity быстро поймут как этим пользоваться.
### Минусы:
Если делать втупую, то получается не супер кэш френдли и все такое. Но в задании сказано не особо переживать на этот счет.
В целом нормально сделать чтобы работало и обкатать API а потом сделать чтобы было быстро если будет сильно надо. 
Но справедливости ради до этого обычно редко доходит и потом все страдают, энивей.

## Event Component System
Хз как это правильно назвать.
Есть Entity - просто идентификаторы.
Есть компоненты - просто структуры данных как в ECS.

Есть система событий.

Например, я хочу задать наложение эффекта пара если юнит в огне и его обливают водой. 
Пишу примерно следующее:

```cpp

enum class Phase {
	Round_Start,
	Unit_Turn_Start,
	Unit_Turn,
	Unit_Turn_End,
	Round_End
};

struct EntityId;

class EventRegistry;
class Commands;

struct EffectWithDuration {
	int remainingDuration{};
};

struct WaterEffect : EffectWithDuration {};

struct FireEffect : EffectWithDuration {
	int damage;
};

struct PoisonEffect : EffectWithDuration {
	int damagePerTurn;
	std::optional<EntityId> source;
};

struct SteamEffect {};

struct DamageState {
	int physical{};
	int fire{};
};

struct SwordsmanBehaviour{};

struct TargetMoveComponent {
	Position target;
};

struct PositionComponent {
	Position position;
};

struct Rending {};

struct World;

void moveToTarget(EventContext& context, TargetMoveComponent& target, PositionComponent& position) {
	commands.move(context.entity, target.target);
}

void registerEventHandlers(EventRegistry& registry) {
	registry.onAdd(Unit_Turn_Start, [](EventContext& context, WaterEffect& water_effect, FireEffect& fire_effect) {
		context.commands.removeEffect<FireEffect>(context.entity);
		context.commands.addEffect<SteamEffect>(context.entity);
	});

	registry.onAdd(Unit_Turn_Start, [](EventContext& context, FireEffect& fire_effect, WaterEffect& water_effect) {
		context.commands.removeEffect<WaterEffect>(context.entity);
		context.commands.addEffect<SteamEffect>(context.entity);
	});

	registry.onTick(Unit_Turn_Start, [](EventContext& context, FireEffect& fire_effect) {
		context.commands.dealDamage(context.entity, DamageState { .fireDamage = fire_effect.damage });
	});

	registry.onDamageDeal([](EventContext& context, FireEffect& fire_effect) {
		context.damageState.fireDamage += fire_effect.damage;
	});

	registry.onTick(Unit_Turn, [](
		EventContext& context,
		SwordsmanBehaviour&,
		PositionComponent& position_component,
		TargetMoveComponent& target_component
	) {
		if (const auto nearby_enemy = world.getEnemyNearby(position_component.position)) {
			if (world.rng.getChance(0.001f)) {
				commands.applyEffect(*nearby_enemy, Rending{});
				commands.dealDamage(*nearby_enemy, DamageState { .physical = 20 });
			} else {
				commands.dealDamage(*nearby_enemy, DamageState { .physical = 10 });
			}
		} else {
			moveToTarget(commands, world, entity, target_component, position_component);
		}
	});

	registry.onAdd([](Commands& commands, World& world, EntityId entity, Rending&) {
		commands.dealDamage(entity, DamageState{ .physical = 5 });
	});

	registry.onDamageReceive([](Commands& commands, World& world, EntityId entity, DamageState& damage_state, PoisonEffect& poison_effect, Rending&) {
		damage_state.poison *= 2;
	});

	registry.onDamageReceive([](Commands& commands, World& world, EntityId entity, DamageState& damage_state, WaterEffect& water_effect) {
		damage_state.fire += 10;
	});
}

```

Как лучше всего комбинировать эффекты и прописывать логики их поведения.

Либо подход описанный выше.
Либо дефолтныый подход:

```cpp
class MovementComponent : ComponentBase {
public:
	void move() {
		Position target_position = _positionComponent->getPosition();

		if (const auto* topological_retard = _entity->getComponent<TopologicalRetard>()) {
			target_position = topological_retard->getNextPosition(target_position);
		}
		else if (const auto* target_component = _entity->getComponent<MoveTargetComponent>()) {
			target_position = target_component->getNextPosition(target_position);
		}

		performMove(target_position);
	}
private:
	Entity* _entity;
	PositionComponent _positionComponent;
};

```
По сути здесь в одном месте описывается логика взаимодействия всех эффектов которые как либо влияют на движение юнита.

Если это переделать на ивенты, то:

```cpp
void performMove(EventContext& context, PositionComponent& position_component) {
	auto target_position = position_component.position;

	if (const auto* component = context.getComponent<TopologicalRetard>(context.entity)) {
		target_position = getNextPosition(*component, target_position);
	} else if (const auto* component = context.getComponent<MoveTargetComponent>(context.entity)) {
		target_position = getNextPosition(*component, target_position);
	}

	position_component.position = target_position;
}

void registerEventHandlers(EventRegistry& registry) {
	registry.onTick(Unit_Turn, [](EventContext& context, CarBehaviour&, PositionComponent& position_component) {
		performMove(context, position_component);
	});

	registry.onAdd(Unit_Turn_Start, [](Commands& commands, World& world, EntityId entity, FireEffect& fire_effect, WaterEffect& water_effect) {
		commands.removeEffect<WaterEffect>(entity);
		commands.addEffect<SteamEffect>(entity);
	});
}
```