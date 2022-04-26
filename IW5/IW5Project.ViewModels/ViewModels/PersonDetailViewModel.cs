using System;
using IW5Project.Model.Persons;
using IW5Project.Services.Services;
using IW5Project.ViewModels.Framework.ViewModels;

namespace IW5Project.ViewModels
{
    public class PersonDetailViewModel: ViewModelCollection<Person>
    {
        public Person PersonDetail { get; private set; }

        public PersonDetailViewModel()
        {

        }
        public PersonDetailViewModel(Guid id)
        {
            Service = new PersonService();
            PersonDetail = Service.GetByID(id);
        }
    }
}
