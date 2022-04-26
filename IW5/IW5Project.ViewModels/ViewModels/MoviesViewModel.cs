using System.Collections.ObjectModel;
using System.Windows.Input;
using IW5Project.Model.Movies;
using IW5Project.Model.Persons;
using IW5Project.Services;
using IW5Project.Services.Repository;
using IW5Project.Services.Services;
using IW5Project.ViewModels.Commands;
using IW5Project.ViewModels.Commands.Movie;
using IW5Project.ViewModels.Framework.ViewModels;

namespace IW5Project.ViewModels
{
    public class MoviesViewModel : ViewModelCollection<Movie>
    {

        public Repository<MoviesDbContext, Person> PersonService { get; set; }
        public Movie MovieDetail { get; private set; }

        public ICommand AddNewMovie { get; set; }
        public ICommand EditCommand { get; set; }
        public ICommand RemoveCommand { get; set; }
        public ICommand InsertPoster { get; set; }

     //   public ObservableCollection<Movie> Movies { get; private set; }

        public MoviesViewModel()
        {
            InitData();
            InitCommands();
        }

        private void InitData()
        {
            this.Name = "Movies";
            base.LoadData();
            this.Service = new MovieService();
            this.PersonService = new PersonService();
            NewItem = null;
            Status = "We weren't able to load data!";
      //      PosterStatus = "Avatar wasn't loaded!";
        }

        private void InitCommands()
        {
            this.AddNewMovie = new AddMovieCommand(this);
            this.EditCommand = new EditMovieCommand<Movie>(this);
            this.RemoveCommand = new RemoveMovieCommand<Movie>(this);
            this.InsertPoster = new InsertPoster(this);
        }
    }
}
