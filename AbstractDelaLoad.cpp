 /*
  * Unitex
  *
  * Copyright (C) 2001-2009 Universit� Paris-Est Marne-la-Vall�e <unitex@univ-mlv.fr>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of the GNU Lesser General Public
  * License as published by the Free Software Foundation; either
  * version 2.1 of the License, or (at your option) any later version.
  *
  * This library is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  * Lesser General Public License for more details.
  *
  * You should have received a copy of the GNU Lesser General Public
  * License along with this library; if not, write to the Free Software
  * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
  *
  */

#include "Unicode.h"
#include "DELA.h"
#include "AbstractDelaLoad.h"
#include "AbstractDelaPlugCallback.h"

struct AbstractDelaSpace {
	t_persistent_dic_func_array func_array;
	void* privateSpacePtr;
} ;


struct List_AbstractDelaSpace {
	AbstractDelaSpace afs;
	List_AbstractDelaSpace* next;
} ;


struct List_AbstractDelaSpace* p_abstract_dela_space_list;



UNITEX_FUNC int UNITEX_CALL AddAbstractDelaSpace(const t_persistent_dic_func_array* func_array,void* privateSpacePtr)
{
	struct List_AbstractDelaSpace* new_item;
	new_item = (struct List_AbstractDelaSpace*)malloc(sizeof(struct List_AbstractDelaSpace));
	if (new_item == NULL)
		return 0;

	new_item->afs.func_array = *func_array;
	new_item->afs.privateSpacePtr = privateSpacePtr;
	new_item->next = NULL;

	if (p_abstract_dela_space_list == NULL)
		p_abstract_dela_space_list = new_item;
	else {
		struct List_AbstractDelaSpace* tmp = p_abstract_dela_space_list;
		while ((tmp->next) != NULL)
			tmp = tmp->next;
		tmp->next = p_abstract_dela_space_list;
	}

	if ((new_item->afs.func_array.fnc_Init_DelaSpace) != NULL)
		(*(new_item->afs.func_array.fnc_Init_DelaSpace))(new_item->afs.privateSpacePtr);

	return 1;
}

UNITEX_FUNC int UNITEX_CALL RemoveAbstractDelaSpace(const t_persistent_dic_func_array* func_array,void* privateSpacePtr)
{
	struct List_AbstractDelaSpace* tmp = p_abstract_dela_space_list;
	struct List_AbstractDelaSpace* tmp_previous = NULL;

	while (tmp != NULL)
	{
		t_persistent_dic_func_array test_func_array = tmp->afs.func_array;
		t_persistent_dic_func_array request_func_array = *func_array;

		if ((memcmp(&tmp->afs.func_array,func_array,sizeof(t_persistent_dic_func_array))==0) &&
			(tmp->afs.privateSpacePtr == privateSpacePtr))
		{
			if (tmp_previous == NULL)
				p_abstract_dela_space_list = tmp->next;
			else
				tmp_previous->next = tmp->next;

			if ((tmp->afs.func_array.fnc_Uninit_DelaSpace) != NULL)
				(*(tmp->afs.func_array.fnc_Uninit_DelaSpace))(tmp->afs.privateSpacePtr);

			free(tmp);
			return 1;
		}
		tmp_previous = tmp;
		tmp = tmp->next;
	}
	return 0;
}

const AbstractDelaSpace * GetDelaSpaceForFileName(const char*name)
{
	const struct List_AbstractDelaSpace* tmp = p_abstract_dela_space_list;

	while (tmp != NULL)
	{
		const AbstractDelaSpace * test_afs = &(tmp->afs);
		if (tmp->afs.func_array.fnc_is_filename_object(name,tmp->afs.privateSpacePtr) != 0)
			return test_afs;		

		tmp = tmp->next;
	}
	return NULL;
}

/*******************************/

struct INF_codes* load_abstract_INF_file(char* name,struct INF_free_info* p_inf_free_info)
{
	struct INF_codes* res = NULL;
	const AbstractDelaSpace * pads = GetDelaSpaceForFileName(name) ;
	if (pads == NULL)
	{
		res = load_INF_file(name);
		if (res != NULL)
		{
			p_inf_free_info->must_be_free = 1;
			p_inf_free_info->func_free_inf = NULL;
			p_inf_free_info->private_ptr = NULL;
		}
		return res;
	}
	else
	{
		p_inf_free_info->must_be_free = 0;
		p_inf_free_info->private_ptr = NULL;
		p_inf_free_info->privateSpacePtr = pads->privateSpacePtr;
		p_inf_free_info->func_free_inf = (void*)(pads->func_array.fnc_free_abstract_INF);
		res = (*(pads->func_array.fnc_load_abstract_INF_file))(name,p_inf_free_info,pads->privateSpacePtr);
		return res;
	}
}

void free_abstract_INF(struct INF_codes* INF,struct INF_free_info* p_inf_free_info)
{
	if (INF != NULL)
		if (p_inf_free_info->must_be_free != 0)
	{
		if (p_inf_free_info->func_free_inf == NULL)
			free_INF_codes(INF);
		else
		{
			t_fnc_free_abstract_INF fnc_free_abstract_INF = (t_fnc_free_abstract_INF)(p_inf_free_info->func_free_inf);
			if (fnc_free_abstract_INF != NULL)
				(*fnc_free_abstract_INF)(INF,p_inf_free_info,p_inf_free_info->privateSpacePtr);
		}
	}
}





unsigned char* load_abstract_BIN_file(char* name,struct BIN_free_info* p_bin_free_info)
{
	unsigned char* res = NULL;
	const AbstractDelaSpace * pads = GetDelaSpaceForFileName(name) ;
	if (pads == NULL)
	{
		res = load_BIN_file(name);
		if (res != NULL)
		{
			p_bin_free_info->must_be_free = 1;
			p_bin_free_info->func_free_bin = NULL;
			p_bin_free_info->private_ptr = NULL;
		}
		return res;
	}
	else
	{
		p_bin_free_info->must_be_free = 0;
		p_bin_free_info->private_ptr = NULL;
		p_bin_free_info->privateSpacePtr = pads->privateSpacePtr;
		p_bin_free_info->func_free_bin = (void*)(pads->func_array.fnc_free_abstract_BIN);
		res = (*(pads->func_array.fnc_load_abstract_BIN_file))(name,p_bin_free_info,pads->privateSpacePtr);
		return res;
	}
}

void free_abstract_BIN(unsigned char* BIN,struct BIN_free_info* p_bin_free_info)
{
	if (BIN != NULL)
		if (p_bin_free_info->must_be_free != 0)
	{
		if (p_bin_free_info->func_free_bin == NULL)
			free(BIN);
		else
		{
			t_fnc_free_abstract_BIN fnc_free_abstract_BIN = (t_fnc_free_abstract_BIN)(p_bin_free_info->func_free_bin);
			if (fnc_free_abstract_BIN != NULL)
			(*fnc_free_abstract_BIN)(BIN,p_bin_free_info,p_bin_free_info->privateSpacePtr);
		}
	}
}
