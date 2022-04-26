using System;
using System.Collections.Generic;
using System.Linq;
using static System.Globalization.CultureInfo;

namespace IW5Project.Model.Persons
{
    public class Person : BaseModel                                                 
    {
        private string _firstName;
        private string _lastName;
        private int _age;
        //        public Guid IdPerson { get; private set; }
        public string Firstname
        {
            get { return _firstName; }
            set
            {
                if (_firstName != value)
                {
                    _firstName= NamesToUpper(value);
                    OnPropertyChanged();
                    OnPropertyChanged("Name");
                }
            } 
        }

        public string Lastname
        {
            get { return _lastName; }
            set
            {
                if (_lastName != value)
                {
                    _lastName = NamesToUpper(value);
                    OnPropertyChanged();
                    OnPropertyChanged("Name");
                }
            }
        }
        public string Avatar { get; private set; }
        public int Age
        {
            get { return _age; }
            set
            {
                if (_age != value)
                {
                    _age = value;
                    OnPropertyChanged();
                }
            }
        }

        public string AvatarEnding { get; private set;}

        public bool IsPosterSet { get; private set;}

        public virtual ICollection<PersonMovieRole> Positions { get; set; }
        

        public Person()
        {
            
        }

        public Person(string firstname, string lastname, int age)
        {
            Firstname = firstname;
            Lastname = lastname;
            Age = age;
        }

        private string NamesToUpper(string str)
        {
            if (string.IsNullOrWhiteSpace(str))
                throw new ArgumentException("Argument is null or whitespace", nameof(str));
            return CurrentCulture.TextInfo.ToTitleCase(str.ToLower());   // at je kazdy jmeno s velkym pismenem
        }

        public string Name => $"{_firstName} {_lastName}";


        public void SetAvatar(string ending) { 
            this.AvatarEnding = ending;
            this.IsPosterSet = true;
        }
        public void DeleteAvatar()
        {
            this.AvatarEnding = "";
            this.IsPosterSet = false;
        }

    public void SetAge(int age)
        {
            if (!Enumerable.Range(1, 110).Contains(age))
                throw new ArgumentException("Argument is out of range <0, 110>", nameof(age));
            Age = age;
        }
    }
}
