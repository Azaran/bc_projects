using System;
using IW5Project.Model.Movies;
using IW5Project.ViewModels.Framework.ViewModels;
using IW5Project.Services.Services;

namespace IW5Project.ViewModels
{
    public class MovieDetailViewModel : ViewModelCollection<Movie>
    {
        public Movie _movieDetail;

        public Movie MovieDetail { get; private set; }
        public MovieDetailViewModel()
        {

        }
        public MovieDetailViewModel(Guid id)
        {
            Service = new MovieService();
            _movieDetail = Service.GetByID(id);

        }
    }
}
