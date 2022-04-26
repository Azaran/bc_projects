using IW5Project.Model.Ratings;
using IW5Project.Services.Repository;

namespace IW5Project.Services.Services
{
    public class RatingService : Repository<MoviesDbContext, Rating>
    {
    }
}
