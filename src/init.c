/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vaghazar <vaghazar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/07 16:28:05 by vaghazar          #+#    #+#             */
/*   Updated: 2022/09/08 13:25:00 by vaghazar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

static int	ft_isdigit(int arg)
{
	if (!(arg <= 57 && arg >= 48))
	{
		return (0);
	}
	return (1);
}

int	valid_args(char **str, int ac)
{
	int	i;
	int	j;

	i = 1;
	j = 0;
	while (i < ac)
	{
		while (str[i][j])
		{
			if ((!ft_isdigit((int)str[i][j]) && str[i][0] != '+' )
				|| (!ft_isdigit((int)str[i][j]) && j != 0))
			{
				printf("Error: not valid\n");
				return (0);
			}
			j++;
		}
		j = 0;
		i++;
	}
	return (1);
}

int	get_args(t_philo	**arg, char **av, int ac)
{
	t_philo	*philo;

	*arg = malloc(sizeof(t_philo) * ft_atoi(av[1]));
	philo = *arg;
	philo->data = (t_var *)malloc(sizeof(t_var));
	(*arg)->data->mutex_fork = malloc(sizeof(t_philo) * ft_atoi(av[1]));
	philo->data->n_philo = ft_atoi(av[1]);
	philo->data->t_to_die = ft_atoi(av[2]);
	philo->data->t_to_eat = ft_atoi(av[3]);
	philo->data->t_to_sleep = ft_atoi(av[4]);
	if (ac == 6)
	{
		philo[0].data->n_t_to_eat = ft_atoi(av[5]);
		if (philo[0].data->n_t_to_eat == 0)
			return (1);
	}
	philo->data->is_dead = 0;
	philo->data->t_start_prog = 0;
	philo->data->philo_ate = 0;
	philo->data->ac = ac;
	// philo->data->pid = getpid();
	// ft_init(philo);
	return (0);
}

int	ft_init(t_philo *philo)
{
	int	i;

	i = -1;
	while (i < philo->data->n_philo)
	{
		philo[i].t_live = 0;
		philo[i].t_n_eat = 0;
		i++;
	}
	pthread_mutex_init(&philo->data->mutex_dead, NULL);
	pthread_mutex_init(&philo->data->mutex_print, NULL);
	i = -1;
	while (++i < philo->data->n_philo)
		pthread_mutex_init(&philo->data->mutex_fork[i], NULL);
	return (0);
}

