/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vaghazar <vaghazar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/03 11:01:25 by vaghazar          #+#    #+#             */
/*   Updated: 2022/08/07 15:54:20 by vaghazar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

long double	get_current_time()
{
	struct timeval time;
	gettimeofday(&time, NULL);
	return ((time.tv_sec * 1000) + (time.tv_usec / 1000));
}

void print_actions(t_philo *philo,char *message)
{
	pthread_mutex_lock(&philo->data->mutex_print);
	philo->data->t_start_prog = philo->data->current_time - philo->data->start_prog;
	printf("%d %d %s %d\n", philo->data->t_start_prog, philo->id, message, philo->t_n_eat);
	if (ft_strncmp(message, "died", 5))
		pthread_mutex_unlock(&philo->data->mutex_print);
}

void *start_cycle(void *arg_philo)
{
	t_philo 	*philo;
	
	philo = arg_philo;
	if (philo->id % 2 == 0)
		usleep(15000);
	while (1)
	{
		pthread_mutex_lock(&philo->data->mutex_fork[philo->fork_l_id]);
		print_actions(philo, "has taken a fork");
		pthread_mutex_lock(&philo->data->mutex_fork[philo->fork_r_id]);
		print_actions(philo, "has taken a fork");
		print_actions(philo, "is eating");
		philo->start_eat = philo->data->current_time;
		philo->t_tmp = 0;
		while (philo->t_tmp < philo->data->t_to_eat && !usleep(200))
			philo->t_tmp = philo->data->current_time - philo->start_eat;
		pthread_mutex_unlock(&philo->data->mutex_fork[philo->fork_l_id]);
		pthread_mutex_unlock(&philo->data->mutex_fork[philo->fork_r_id]);
		philo->t_live = 0;
		philo->start_live = philo->data->current_time;
		print_actions(philo, "is sleeping");
		philo->t_n_eat++;
		philo->start_sleep = philo->data->current_time;
		philo->t_tmp = 0;
		while (philo->t_tmp < philo->data->t_to_sleep && !usleep(200))
			philo->t_tmp = philo->data->current_time - philo->start_sleep;
		print_actions(philo, "is thinking");
	}
	return (0);
}

void get_args(t_philo	**arg, char **av, int ac)
{
	t_philo	*philo;
	
	*arg = malloc(sizeof(t_philo) * ft_atoi(av[1]));
	philo = *arg;
	philo[0].data = malloc(sizeof(t_var));
	(*arg)->data->mutex_fork = malloc(sizeof(t_philo) * ft_atoi(av[1]));
	philo[0].data->n_philo = ft_atoi(av[1]);
	philo[0].data->t_to_die = ft_atoi(av[2]);
	philo[0].data->t_to_eat = ft_atoi(av[3]);
	philo[0].data->t_to_sleep = ft_atoi(av[4]);
	if (ac == 6)
		philo[0].data->n_t_to_eat = ft_atoi(av[5]);
	philo->data->is_dead = 0;
	philo->data->t_start_prog = 0;
	philo->data->philo_ate = 0;
	zero_init(&philo);
	philo->data->ac = ac;
}

int	create_threads(t_philo *philo)
{
	int		i;
	
	i = -1;
	while (++i < philo->data->n_philo)
	{
		philo[i].data = philo[0].data;
		philo[i].fork_l_id = i;
		if (i == philo->data->n_philo - 1)
			philo[i].fork_r_id = 0;
		else
			philo[i].fork_r_id = i + 1;
		philo[i].id = i + 1;
	}
	philo->data->start_prog = get_current_time();
	philo->data->current_time = philo->data->start_prog;
	i = -1;
	if (philo->data->ac == 6)
		pthread_create(&philo->data->eat_ptid, NULL, &ft_check_eat, philo);
	while (++i < philo->data->n_philo)
	{
		philo[i].start_live = philo->data->start_prog;
		pthread_create(&philo[i].ptid, NULL, &start_cycle, &philo[i]);
	}
	return (0);
}



int	ft_destroy_all(t_philo *philo)
{
	int	i;

	i = 0;
	pthread_mutex_destroy(&philo->data->mutex_dead);
	pthread_mutex_destroy(&philo->data->mutex_print);
	while (i < philo->data->n_philo)
	{
		pthread_mutex_destroy(&philo->data->mutex_fork[i++]);
	}
	free(philo->data->mutex_fork);
	free(philo->data);
	free(philo);
	return (0);
}

int	ft_init(t_philo *philo)
{
	int	i;

	i = -1;
	pthread_mutex_init(&philo->data->mutex_dead, NULL);
	pthread_mutex_init(&philo->data->mutex_print, NULL);
	while(++i < philo->data->n_philo)
		pthread_mutex_init(&philo->data->mutex_fork[i], NULL);
	return (0);
}

int	dead_check(t_philo *philo)
{
	int	i;
	
	while (!philo->data->philo_ate && !philo->data->is_dead)
	{
		i = 0;
		while (i < philo->data->n_philo)
		{
			philo->data->current_time = get_current_time();
			philo[i].t_live = philo->data->current_time - philo[i].start_live;
			if (philo->data->t_to_die < philo[i].t_live)
			{
				print_actions(&philo[i], "died");
				philo->data->is_dead = 1;
				break;
			}
			i++;
		}
	}
	return (0);
}

int main(int ac, char **av)
{
	t_philo 		*philo;
	int	i;
	
	if (ac == 5 || ac == 6)
	{
		if (!valid_args(av, ac))
			return (1);
		get_args(&philo, av, ac);
		ft_init(philo);
		create_threads(philo);
		dead_check(philo);
	}
	else
		printf("Error: usage -> number_of_philosophers time_to_die time_to_eat\
 time_to_sleep [number_of_times_each_philosopher_must_eat]\n");
	return (0);
}