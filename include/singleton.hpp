/**
 * @file singleton.hpp
 * @author Gianni Peschiutta (F4IKZ)
 * @brief HAM-ESP32-RPT
 * @version 0.1
 * @date 2025-02-13
 * 
 * @copyright Copyright (c) 2025
 *
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *    This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 *    You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>
 */

#ifndef CSINGLETON_HPP
#define CSINGLETON_HPP

/**
 * @brief Singleton template 
 * 
 * @tparam T 
 */
template <class T> class CSingleTon
{
public :

    /**
     * @brief Creat instance of singleton if not exist, and anyway , it return instance
     * 
     * @return T* 
     */
    static	T*	Create()
    {
        if (!Inst)
            Inst = new T;

        return Inst;
    }

    /**
     * @brief Destruct the singleton instance if exist
     * 
     */
    static	void	Kill()
    {
        delete Inst;
        Inst = NULL;
    }

protected :

    //----------------------------------------------------------------
    // Constructeur par défaut
    //----------------------------------------------------------------
    CSingleTon() {}

    //----------------------------------------------------------------
    // Destructeur
    //----------------------------------------------------------------
    ~CSingleTon() {}

private :

    //----------------------------------------------------------------
    // Données membres
    //----------------------------------------------------------------
    static	T*	Inst; // Instance de la classe

    //----------------------------------------------------------------
    // Copie interdite
    //----------------------------------------------------------------
    CSingleTon(CSingleTon&);
    void operator =(CSingleTon&);
};

//----------------------------------------------------------------
// Déclaration de notre variable statique
//----------------------------------------------------------------
template <class T> T* CSingleTon<T>::Inst = NULL;

#endif //CSINGLETON_HPP
