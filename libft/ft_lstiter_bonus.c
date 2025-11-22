/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstiter_bonus.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: joleksia <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/08 09:15:41 by joleksia          #+#    #+#             */
/*   Updated: 2025/05/08 10:18:12 by joleksia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstiter(t_list *lst, void (*f)(void *))
{
	t_list	*_lst;

	if (!lst)
		return ;
	_lst = lst;
	while (lst)
	{
		lst = lst->next;
		f(_lst->content);
		_lst = lst;
	}
}
