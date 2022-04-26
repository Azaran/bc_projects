using System.Collections.ObjectModel;
using IW5Project.Model.Ratings;

namespace IW5Project.ViewModels
{
    public class MovieRating
    {

        private Rating _movieDetail;

        public Rating MovieDetail { get; private set; }
        public ObservableCollection<Rating> RatingsToMovie { get; private set; }

        public MovieRating()
        {
        }
    }
}
