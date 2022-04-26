using System.Collections.ObjectModel;
using System.Windows.Forms;
using System.Windows.Input;
using IW5Project.Model.Movies;
using IW5Project.Model.Persons;
using IW5Project.Services;
using IW5Project.Services.Repository;
using IW5Project.Services.Services;
using IW5Project.ViewModels.Commands;
using IW5Project.ViewModels.Commands.Person;
using IW5Project.ViewModels.Framework.ViewModels;

namespace IW5Project.ViewModels
{
    public class PersonsViewModel : ViewModelCollection<Person>
    {
        public  Repository<MoviesDbContext, Movie> MovieService { get; set; }

        public ICommand AddNewPerson { get; set; }
        public ICommand EditCommand { get; set; }
        public ICommand RemoveCommand { get; set; }
        public ICommand InsertAvatar { get; set; }


        public PersonsViewModel()
        {
            InitData();
            InitCommands();
        }
        private void InitCommands()
        {
            this.AddNewPerson = new AddPersonCommand(this);
            this.EditCommand = new EditPersonCommand<Person>(this);
            this.RemoveCommand = new RemovePersonCommand<Person>(this);
            this.InsertAvatar = new InsertAvatar(this);
           
        }
        private void InitData()
        {
            this.Name = "Persons";
            this.Service = new PersonService();
            this.MovieService = new MovieService();
            base.LoadData();
            this.NewItem = null;
            Status = "We weren't able to load data!";
            AvatarStatus = "Avatar wasn't loaded!";
        }
    }
}